#ifndef KT_APP_MSG_H_
#define KT_APP_MSG_H_

namespace kt {

/**
 * @class kt::Msg
 * @brief Abstract message class. The base classes can receiving messages,
 * but currently sending messages requires a service (i.e. the message sending
 * system is decoupled from the core application, as an experiment).
 */
class Msg {
public:
	Msg() = delete;
	Msg(const Msg&) = delete;
	virtual ~Msg()				{ }

	const size_t				mWhat;

protected:
	Msg(const size_t what)	: mWhat(what) { }
};

} // namespace kt

#endif
