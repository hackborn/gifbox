#include "button_behavior.h"

#include <kt/view/view_defs.h>

namespace kt {

/**
 * @class kt::ButtonBehavior
 */
ButtonBehavior::ButtonBehavior() {
}

void ButtonBehavior::pointerDown(const Pointer&) {
	if (mVisualStateFn) mVisualStateFn(kt::view::DOWN_STATE);
}

void ButtonBehavior::pointerMoved(const Pointer&, const std::uint32_t flags) {
}

void ButtonBehavior::pointerUp(const Pointer&) {
	if (mVisualStateFn) mVisualStateFn(kt::view::REST_STATE);
	if (mClickFn) mClickFn();
}

} // namespace kt

