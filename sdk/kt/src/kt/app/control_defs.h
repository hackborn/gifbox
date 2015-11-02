#ifndef KT_APP_CONTROLDEFS_H_
#define KT_APP_CONTROLDEFS_H_

#include <cstdint>

namespace kt {

// EXECUTION
enum class Execution		{ kContinue, kStop };

// PRIORITY
using priority = std::int32_t;

static const priority		CRITICAL_PRIORITY = -100;
static const priority		SYSTEM_PRIORITY = -50;
static const priority		STANDARD_PRIORITY = 0;
static const priority		LOW_PRIORITY = 50;

} // namespace kt

#endif
