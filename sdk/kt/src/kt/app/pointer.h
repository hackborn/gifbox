#ifndef KT_APP_POINTER_H_
#define KT_APP_POINTER_H_

#include <cstdint>

namespace kt {

/**
 * @class kt::Pointer
 * @brief A pointer (touch/mouse etc.) event. Loosely follows the
 * W3C Pointer Event specification.
 */
class Pointer {
public:
	Pointer() { }

	bool					isPrimary() const { return (mButtons&(1<<0)) != 0; }
	bool					isSecondary() const { return (mButtons&(1<<1)) != 0; }
	bool					isTertiary() const { return (mButtons&(1<<2)) != 0; }

	std::int32_t			mId = 0;
	float					mX = 0.0f,
							mY = 0.0f;
	// Corresponds to mouse buttons ((1<<0) for primary, (1<<1) for secondary etc.).
	std::uint32_t			mButtons = 0;
};

} // namespace kt

#endif
