#include "gif_view.h"

#include <cinder/gl/draw.h>
#include <cinder/gl/gl.h>
#include <cinder/gl/scoped.h>

namespace cs {

namespace {
// This default GIF frame rate seems weird, but matches what some other apps are doing.
const double				DEFAULT_FPS = (7.0/60.0);
}

/**
 * @class cs::GifView
 */
GifView::GifView(kt::Cns &cns, const glm::ivec2 &window_size)
		: base(cns) {
	// Create the mesh
	const glm::vec2			tc_ul(0.0f, 0.0f),
							tc_ur(1.0f, 0.0f),
							tc_lr(1.0f, 1.0f),
							tc_ll(0.0f, 1.0f);
	const float				fw = static_cast<float>(window_size.x),
							fh = static_cast<float>(window_size.y);
	ci::gl::VboMeshRef		mesh = ci::gl::VboMesh::create(ci::geom::Rect(ci::Rectf(0, 0, fw, fh)).texCoords(tc_ul, tc_ur, tc_lr, tc_ll));
	if (!mesh) throw std::runtime_error("Background vbo can't create vbo mesh");

	// Create shader
	auto					shader = ci::gl::getStockShader(ci::gl::ShaderDef().texture());
	if (!shader) throw std::runtime_error("Background vbo can't create shader");
	
	// Create batch
	mBatch = ci::gl::Batch::create(mesh, shader);
	if (!mBatch) throw std::runtime_error("Background vbo can't create batch");
}

void GifView::setTextures(const TextureGifList &t) {
	mTextures = t;
	mTextureIndex = 0;
	mNextTime = findFrameRate();
	mTimer.start();
}

void GifView::setPlaybackSpeed(const float s) {
	mNextPlaybackSpeed = s;
}

void GifView::onUpdate(const kt::UpdateParams&) {
	if (!mTextures.empty()) {
		const double		elapsed = mTimer.elapsed();
		if (elapsed >= mNextTime) {
			if (++mTextureIndex >= mTextures.size()) {
				mTextureIndex = 0;
			}
			restartTimer();
		} else if (mNextPlaybackSpeed != mPlaybackSpeed) {
			mPlaybackSpeed = mNextPlaybackSpeed;
			restartTimer();
		}
	}
}

void GifView::onDraw(const kt::view::DrawParams&) {
	auto*			frame = mTextures.getFrame(mTextureIndex);
	if (frame && frame->mBitmap) {
		ci::gl::color(1, 1, 1, 1);
		ci::gl::ScopedTextureBind	ts(frame->mBitmap);
		ci::gl::ScopedDepth			sd(false);
		mBatch->draw();
	}
}

void GifView::restartTimer() {
	mNextTime = findFrameRate();

	// Modulate by current playback speed.
	if (mPlaybackSpeed <= 0.0f) {
		mNextTime = std::numeric_limits<float>::max();
	} else {
		mNextTime *= (1.0f/mPlaybackSpeed);
	}

	mTimer.start();
}

double GifView::findFrameRate() const {
	auto*			frame = mTextures.getFrame(mTextureIndex);
	if (frame && frame->mDelay > 0.00000001) return frame->mDelay;
	return DEFAULT_FPS;
}

} // namespace cs
