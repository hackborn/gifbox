#ifndef APP_THREAD_SAFEVECTOR_H_
#define APP_THREAD_SAFEVECTOR_H_

#include <vector>
#include <cinder/Thread.h>

namespace cs {

/**
 * @class cs::SafeVector
 * @brief Simple thread-safe vector.
 * @description Manage a vector of data, allowing items to be pushed
 * or the whole vector to be popped.
 */
template <typename T>
class SafeVector {
public:
	using Container = std::vector<T>;

	bool					push_back(const T&);
	bool					pop_all(Container&);

private:
	std::mutex				mMutex;
	Container				mContainer;
};

/**
 * @class cs::SafeVector Implementation
 */
template <typename T>
bool SafeVector<T>::push_back(const T &t) {
	try {
		std::lock_guard<std::mutex> lock(mMutex);
		mContainer.push_back(t);
		return true;
	} catch (std::exception const&) {
	}
	return false;
}

template <typename T>
bool SafeVector<T>::pop_all(Container &c) {
	try {
		std::lock_guard<std::mutex> lock(mMutex);
		if (!mContainer.empty()) {
			c.reserve(c.size() + mContainer.size());
			c.insert(c.end(), mContainer.begin(), mContainer.end());
			mContainer.clear();
		}
		return true;
	} catch (std::exception const&) {
	}
	return false;
}

} // namespace cs

#endif