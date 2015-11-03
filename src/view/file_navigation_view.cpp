#include "file_navigation_view.h"

#include <cinder/gl/draw.h>
#include <cinder/gl/gl.h>
#include "msg/app_msg.h"

namespace cs {

/**
 * @class cs::FileNavigationView
 */
FileNavigationView::FileNavigationView(kt::Cns &cns)
		: base(cns) {
	setPointerEvents(true);
}

void FileNavigationView::setNavigation(FileNavigationRef r) {
	mNavigation = r;
}

void FileNavigationView::pointerDown(const kt::Pointer&) {
}

void FileNavigationView::pointerUp(const kt::Pointer&) {
	if (mNavigation) {
		auto		next = mNavigation->next();
		if (!next.empty()) {
			cs::SetMediaPathMsg(next).send(mCns);
		}
	}
}

void FileNavigationView::onDraw(const kt::view::DrawParams&) {
	ci::gl::color(0.0f, 1.0f, 0.0f, 1.0f);
	ci::gl::drawSolidRect(ci::Rectf(0.0f, 0.0f, mSize.x, mSize.y));	
}

} // namespace cs
