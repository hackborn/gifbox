#ifndef APP_VIEW_MEDIAVIEW_H_
#define APP_VIEW_MEDIAVIEW_H_

#include <kt/view/view.h>

namespace cs {

/**
 * @class cs::MediaView
 * @brief Abstract container to display media.
 */
class MediaView : public kt::view::View {
public:
	MediaView() = delete;
	MediaView(const MediaView&) = delete;
	MediaView(kt::Cns&);

	template <typename T>
	T&						setMediaViewOrThrow(T*);

	// Controlled by the application -- set the current relative view size, 0=min, 1=max.
	void					setMediaSize(const float = 0.0f);

protected:
	void					onMsg(const kt::Msg&) override;
	void					onSizeChanged(const glm::vec3&) override;

private:
	void					positionMediaView();
	void					scaleMediaView();

	using base = kt::view::View;

	kt::view::ViewRef		mMediaView;
	// The global relative scale value for the media, 0=min, 1-max.
	float					mMediaScale = 0.0f;
};

/**
 * MediaView Implementation
 */
template <typename T>
T& MediaView::setMediaViewOrThrow(T *view) {
	if (!view) throw std::runtime_error("MediaView::setMediaViewOrThrow() on nullptr");
	if (mMediaView) removeChild(mMediaView);
	mMediaView.reset(view);
	addChild(mMediaView);
	positionMediaView();
	return *view;
}

} // namespace cs

#endif
