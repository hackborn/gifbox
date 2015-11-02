#ifndef APP_VIEW_GIFVIEW_H_
#define APP_VIEW_GIFVIEW_H_

#include <cinder/gl/Batch.h>
#include <kt/time/seconds.h>
#include <kt/view/view.h>
#include "cinder_gif/texture_gif_list.h"

namespace cs {

/**
 * @class cs::GifView
 * @brief Draw the gif list.
 */
class GifView : public kt::view::View {
public:
	GifView() = delete;
	GifView(const GifView&) = delete;
	GifView(kt::Cns&, const glm::ivec2 &window_size);

	void					setTextures(const TextureGifList&);
	size_t					getCurrentFrame() const { return mTextureIndex; }
	void					setPlaybackSpeed(const float = 1.0f);

protected:
	void					onUpdate(const kt::UpdateParams&) override;
	void					onDraw(const kt::view::DrawParams&) override;

private:
	void					restartTimer();
	// Answer the frame rate of the current frame, or the default.
	double					findFrameRate() const;

	using base = kt::view::View;
	TextureGifList			mTextures;
	size_t					mTextureIndex = 0;
	kt::time::Seconds		mTimer;
	double					mNextTime;
	float					mPlaybackSpeed = 1.0f,
							mNextPlaybackSpeed = 1.0f;
	ci::gl::BatchRef		mBatch;
};

} // namespace cs

#endif
