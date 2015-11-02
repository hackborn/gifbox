#include "msg_service.h"

#include <iostream>
#include <sstream>
#include <kt/app/cns.h>

namespace kt {
namespace msg {

/**
 * @class kt::msg::Client
 */
Client::Client(Cns &cns)
		: mService(cns.getService<MsgService>([&cns]()->MsgService*{return new MsgService(cns);})) {
}

Client::~Client() {
	for (auto& it : mInUse) {
		if (it) it->remove(this);
	}
}

void Client::inUse(Channel *c) {
	if (!c) return;
	if (std::find(mInUse.begin(), mInUse.end(), c) != mInUse.end()) {
		mInUse.push_back(c);
	}
}

/**
 * @class kt::msg::Channel
 */
void Channel::add(const void *ptr, const msg_fn &fn) {
	if (!ptr || !fn) return;
	try {
		mAdds[ptr] = fn;
		if (mServiceDirty) *mServiceDirty = true;
	} catch (std::exception const&) {
	}
}

namespace {
bool		remove_one(std::map<const void*, msg_fn> &m, const void *ptr) {
	for (auto it = m.begin(), end = m.end(); it != end; ++it) {
		if (it->first == ptr) {
			m.erase(it);
			return true;
		}
	}
	return false;
}

void		remove_all(std::map<const void*, msg_fn> &m, const void *ptr) {
	while (remove_one(m, ptr)) ;
}
}

void Channel::remove(const void *ptr) {
	if (!ptr) return;
	try {
		// Null out targets
		for (auto& it : mTargets) {
			if (it.first == ptr) it.second = nullptr;
		}
		// Remove from adds
		remove_all(mAdds, ptr);
		if (mServiceDirty) *mServiceDirty = true;
	} catch (std::exception const&) {
	}
}

void Channel::applyEdits() {
	try {
		// Remove empties
		remove_all(mTargets, nullptr);
		// Apply all adds
		if (!mAdds.empty()) {
			for (const auto& e : mAdds) {
				mTargets[e.first] = e.second;
			}
			mAdds.clear();
		}
	} catch (std::exception const&) {
	}
}

/**
 * @class kt::MsgService
 */
const std::string& MsgService::NAME() {
	static const std::string	N("kt/msg1");
	return N;
}

MsgService::MsgService(kt::Cns &cns) {
}

Channel& MsgService::getChannel(TargetId id) {
	Channel&	c = mChannels[id];
	c.mServiceDirty = &mChannelsDirty;
	return c;
}

void MsgService::applyEdits() {
	if (!mChannelsDirty) return;
	try {
		for (auto& it : mChannels) it.second.applyEdits();
		mChannelsDirty = false;
	} catch (std::exception const&) {
	}
}

/**
 * @class kt::msg::Registry
 */
long Registry::add(const std::string &name) {
	size_t		ans = mMsgs.size() + 1;
	mMsgs[ans] = name;
	return ans;
}

void Registry::report() {
	std::cout << "rf::msg::Registry size=" << mMsgs.size() << std::endl;
	for (const auto& e : mMsgs) {
		std::cout << "\t" << e.first << ": " << e.second << std::endl;
	}
}

/**
 * @class kt::msg::Registry::Entry
 */
Registry::Entry::Entry(const std::string &name)
		: mWhat(Registry::get().add(name))
		, mName(name) {
}

} // namespace msg
} // namespace kt
