#ifndef KT_APP_UPDATEDEFS_H_
#define KT_APP_UPDATEDEFS_H_

#include <chrono>

namespace kt {
using app_time = std::chrono::time_point<std::chrono::system_clock>;

/**
 * \class kt::UpdateParams
 */
class UpdateParams {
public:
	UpdateParams() : mLastTime(mTime) { }

	// Universal time at the start of the current frame.
	kt::app_time			mTime = std::chrono::system_clock::now();
	// Time of the previous update
	kt::app_time			mLastTime;
	// Elapsed seconds between last time and time
	double					mElapsed = 0.0;
};

} // namespace kt

#endif
