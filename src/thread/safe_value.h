#ifndef APP_THREAD_SAFEVALUE_H_
#define APP_THREAD_SAFEVALUE_H_

#include <mutex>

namespace cs {

/**
 * @class cs::SafeValue
 * @brief Simple class to handle shuttling a single value between threads.
 */
template <typename T>
class SafeValue {
public:
	// As a convenience, create a new ref on my data object
	static std::shared_ptr<T>	make() { return std::make_shared<T>(); }

	SafeValue() { }

	void						push(const std::shared_ptr<T> &v) {
		std::lock_guard<std::mutex> lock(mMutex);
		mValue = v;
	}

	std::shared_ptr<T>			pop() {
		std::lock_guard<std::mutex> lock(mMutex);
		auto					a = mValue;
		mValue.reset();
		return a;
	}

private:
	std::mutex					mMutex;
	std::shared_ptr<T>			mValue;
};

} // namespace cs

#endif