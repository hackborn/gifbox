#ifndef KT_TIME_SECONDS_H_
#define KT_TIME_SECONDS_H_

#include <chrono>

namespace kt {
namespace time {

/**
 * \class kt::time::Seconds
 * \brief A timer based on seconds. Not valid until start() is called.
 */
class Seconds {
public:
	Seconds();

	// Self-contained API.
	void						start();
	void						start(const double offset_seconds /* negative value to start at earlier than current titme */);
	double						elapsed() const;
	// API for when someone outside of me is tracking the current time.
	void						start(const std::chrono::time_point<std::chrono::system_clock> &start_time);
	double						elapsed(const std::chrono::time_point<std::chrono::system_clock> &end_time) const;
    
private:
	std::chrono::time_point<std::chrono::system_clock>
								mStart;
};

} // namespace time
} // namespace kt

#endif
