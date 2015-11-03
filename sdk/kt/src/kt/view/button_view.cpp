#include "button_view.h"

#include <kt/app/cns.h>

namespace kt {
namespace view {

/**
 * @class kt::view::Button
 */
Button::Button(kt::Cns &cns)
		: base(cns) {
	setPointerEvents(true);
}

void Button::setVisual(const VisualRef &v) {
	mVisual = v;
	if (mVisual) {
		mVisual->setSize(mSize);
		mVisual->setState(mLastVisualState);
	}
}

void Button::setVisualState(const std::string &s) {
	if (s == mLastVisualState) return;

	mLastVisualState = s;
	if (mVisual) mVisual->setState(s);
}

void Button::setClickFn(std::function<void(void)> fn) {
	mOnClickFn = fn;
}

void Button::pointerUp(const kt::Pointer&) {
	if (mOnClickFn) mOnClickFn();
}

void Button::onDraw(const kt::view::DrawParams &p) {
	base::onDraw(p);
	if (mVisual) mVisual->onDraw(p);
}

void Button::onSizeChanged(const glm::vec3 &size) {
	base::onSizeChanged(size);
	if (mVisual) mVisual->setSize(size);
}

} // namespace view
} // namespace kt

