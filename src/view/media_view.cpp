#include "media_view.h"

#include "msg/view_msg.h"

namespace cs {

/**
 * @class cs::MediaView
 */
MediaView::MediaView(kt::Cns &cns)
		: base(cns) {
}

void MediaView::setMediaSize(const float s) {
	mMediaScale = s;
	scaleMediaView();
}

void MediaView::onMsg(const kt::Msg &_m) {
	if (_m.mWhat == cs::MediaChangedMsg::WHAT()) {
		scaleMediaView();
	}
}

void MediaView::onSizeChanged(const glm::vec3 &size) {
	base::onSizeChanged(size);
	positionMediaView();
}

void MediaView::positionMediaView() {
	if (mMediaView) {
		mMediaView->setCenter(0.5f, 0.5f);
		mMediaView->setPosition(floorf(getSize().x/2.0f), floorf(getSize().y/2.0f));
	}
}

void MediaView::scaleMediaView() {
	if (!mMediaView) return;

	const auto		mv_size(mMediaView->getSize());
	const float		max_sw = mSize.x / mv_size.x,
					max_sh = mSize.y / mv_size.y;
	const float		max_scale = fminf(max_sw, max_sh);
	const float		scale = 1.0f + (mMediaScale * max_scale);
	mMediaView->setScale(scale, scale);
}

} // namespace cs
