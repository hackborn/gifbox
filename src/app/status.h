#ifndef APP_APP_STATUS_H_
#define APP_APP_STATUS_H_

#include <string>
#include <cinder/Thread.h>

namespace cs {

/**
 * @class cs::Status
 * @brief Store the current app status.
 */
class Status {
public:
	enum class Type				{ kError, kInfo };
	enum class Duration			{ kOneShot, kStart, kEnd };

	static Status				makeError(std::string msg) {
		Status ans;
		ans.mType = Type::kError;
		ans.mMessage = msg;
		return ans;
	}
	
	Status() { }
	Status(Duration t, std::string m) : mDuration(t), mMessage(m) { }
	Status(Duration t, uint32_t id, std::string m) : mDuration(t), mId(id), mMessage(m) { }

	Type					mType = Type::kInfo;
	Duration				mDuration = Duration::kOneShot;
	// Currently IDs are only used to associate start and end items.
	uint32_t				mId = 0;
	std::string				mMessage;
};

} // namespace cs

#endif