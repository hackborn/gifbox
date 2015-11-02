#include "seconds.h"

namespace kt {
namespace time {

/**
 * @class kt::time::Seconds
 */
Seconds::Seconds()
		: mStart(std::chrono::system_clock::now()) {
}

void Seconds::start() {
	start(std::chrono::system_clock::now());
}

void Seconds::start(const double offset_seconds) {
	start();
	auto		sec(std::chrono::milliseconds((long)(offset_seconds*1000.0)));
	mStart += sec;
}

double Seconds::elapsed() const {
	return elapsed(std::chrono::system_clock::now());
}

void Seconds::start(const std::chrono::time_point<std::chrono::system_clock> &start_time) {
	mStart = start_time;
}

double Seconds::elapsed(const std::chrono::time_point<std::chrono::system_clock> &end_time) const {
	auto diff = end_time - mStart;
	auto sec = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
	return (static_cast<double>(sec.count())/1000.0);
}

} // namespace time
} // namespace kt