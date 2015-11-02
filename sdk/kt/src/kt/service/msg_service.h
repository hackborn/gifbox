#ifndef KT_SERVICE_MSGSERVICE_H_
#define KT_SERVICE_MSGSERVICE_H_

#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <kt/app/cns.h>
#include <kt/app/msg.h>
#include <kt/app/service.h>

namespace kt {
namespace msg {
class Channel;
class MsgService;
using msg_fn = std::function<void(const kt::Msg&)>;
using TargetId = std::pair<size_t, std::string>;

/**
------------------------------------------------
 MESSAGING SYSTEM
------------------------------------------------
For clients, there are three main pieces:
* Creating a message
* Sending a message
* Receiving a message

-------------------
 CREATING A MESSAGE
-------------------
Create a message by deriving from RegisteredMsg. For example:
 
class ChangeMsg : public kt::msg::RegisteredMsg<ChangeMsg> {
public:
	ChangeMsg() { }
};

or

struct ChangeMsg : public kt::msg::RegisteredMsg<ChangeMsg> { };

--------------------
 RECEIVING A MESSAGE
--------------------
Create a Client class.

	kt::msg::Client		mC;

Add handlers.

	mC.add<SomeMsg>([](const SomeMsg &m){ });


-----------------
 SENDING A MESSGE
-----------------

Use the send function on an existing (or in-line created) msg, passing it
the global CNS. For example:

ChangeMsg().send(cns);

*/


/**
 * @class kt::msg::Client
 * @brief Provide access to receiving messages.
 */
class Client {
public:
	Client() = delete;
	Client(const Client&) = delete;
	Client(Cns&);
	virtual ~Client();

	template <typename T>
	void			add(const std::function<void(const T&)>&);
	// If you want to use something besides the default message channel, use this.
	template <typename T>
	void			add(const std::string &channel, const std::function<void(const T&)>&);

private:
	void					inUse(Channel*);

	MsgService&				mService;
	// Store the channels I add to
	std::vector<Channel*>	mInUse;
};

/**
 * @class kt::msg::Channel
 * @brief Store all targets for a channel.
 */
class Channel {
public:
	Channel() { }

	void				add(const void*, const msg_fn&);
	void				remove(const void*);
	template <typename T>
	void				send(const T&);

	void				applyEdits();

private:
	friend class MsgService;
	// Owning MsgService installs this so I can signify when I have edits.
	bool*				mServiceDirty = nullptr;
	std::map<const void*, msg_fn>
						mTargets;
	// Edits only happen on an update
	std::map<const void*, msg_fn>
						mAdds;
};

/**
 * @class kt::MsgService
 * @brief A service for managing message sending.
 */
class MsgService : public Service {
public:
	static const std::string&	NAME();
	MsgService() = delete;
	MsgService(const MsgService&) = delete;
	MsgService(kt::Cns&);

	// Creates channel if needed. Will throw if anything goes wrong.
	Channel&		getChannel(TargetId);

public:
	// A sending utility, so I can flush any edits.
	class SendLock {
	public:
		SendLock(MsgService &s) : mService(s) {
			if (mService.mSendDepth < 1) mService.applyEdits();
			++mService.mSendDepth;
		}

		~SendLock() {
			--mService.mSendDepth;
		}

	private:
		MsgService&	mService;
	};

private:
	void			applyEdits();

	std::map<TargetId, Channel>
					mChannels;
	// Any channel that has edits will set this
	bool			mChannelsDirty = false;
	size_t			mSendDepth = 0;
};

/**
 * @class kt::msg::Registry
 * @brief Store all registered message types. This is an internal-only
 * object; entries are made automatically by constructing new
 * RegisteredMsg subclasses.
 */
class Registry {
public:
	static Registry&			get() { static Registry	REGISTRY; return REGISTRY; }
	Registry() { }

	long						add(const std::string&);

	// Print out all registered messages
	void						report();

public:
	class Entry {
	public:
		Entry(const std::string &name);

		size_t					getWhat() const { return mWhat; }

		MsgService*				mService = nullptr;
		Channel*				mDefaultChannel = nullptr;

	private:
		const size_t			mWhat;
		const std::string		mName;
	};

private:
	std::map<long, std::string>	mMsgs;
};

/**
 * @class kt::msg::RegisteredMsg
 * @brief All messages should derive from this class, to be properly
 * registered with the system.
 */
template<class Derived>
class RegisteredMsg : public kt::Msg {
public:
	// Unique identifier for this message
	static size_t				WHAT() { return sENTRY.getWhat(); }

	// Utility to send this msg over its own channel.
	void						send(Cns &cns) { send(std::string(), cns); }
	void						send(const std::string &channel, Cns &cns);
	
protected:
	RegisteredMsg() : Msg(sENTRY.getWhat()) { }

private:
	static Registry::Entry		sENTRY;
};

template<class Derived>
Registry::Entry RegisteredMsg<Derived>::sENTRY(typeid(Derived).name());

/**
 * KT::MSG::CHANNEL IMPLEMENTATION
 */
template<typename T>
void Channel::send(const T &m) {
	for (const auto& pair : mTargets) {
		std::function<void(const T&)> local_fn = *reinterpret_cast<msg_fn const *>(&pair.second);
		local_fn(m);
	}
}

/**
 * KT::MSG::REGISTERED-MSG IMPLEMENTATION
 */
template <typename T>
void RegisteredMsg<T>::send(const std::string &channel, Cns &cns) {
	if (!sENTRY.mService) {
		sENTRY.mService = &(cns.getService<MsgService>());
		if (!sENTRY.mService) return;
	}
	// Optimize the default channel
	if (channel.empty()) {
		if (!sENTRY.mDefaultChannel) {
			TargetId		id(mWhat, channel);
			sENTRY.mDefaultChannel = &(sENTRY.mService->getChannel(std::move(id)));
			if (!sENTRY.mDefaultChannel) return;
		}
		MsgService::SendLock(*sENTRY.mService);
		sENTRY.mDefaultChannel->send<T>((const T&)(*this));
		return;
	}
	// XXX get for the channel
}

/**
 * KT::MSG::CLIENT IMPLEMENTATION
 */
template <typename T>
void Client::add(const std::function<void(const T&)> &fn) {
	add(std::string(), fn);
}

template <typename T>
void Client::add(const std::string &channel, const std::function<void(const T&)> &fn) {
	if (!fn) return;

	try {
		TargetId		id(T::WHAT(), channel);
		Channel&		c(mService.getChannel(std::move(id)));
		inUse(&c);
		auto			casted = *reinterpret_cast<msg_fn const *>(&fn);
		c.add(this, casted);
	} catch (std::exception const&) {
	}
}

} // namespace msg
} // namespace kt

// Make the resource ID available for hashing functions
namespace std {
template<>
struct hash<kt::msg::TargetId> : public unary_function < kt::msg::TargetId, size_t > {
	size_t operator()(const kt::msg::TargetId& id) const {
		std::hash<std::string> hash_fn;
		std::size_t str_hash = hash_fn(id.second);
		return str_hash + id.first;
	}
};
}

#endif
