#include "texture_gif_list.h"

#include <stdexcept>
#include <cinder/ImageIo.h>
#include <kt/app/environment.h>

namespace cs {

/**
 * @class cs::TextureGifList
 */
TextureGifList::TextureGifList()
		: base([this](const gif::Bitmap &bm)->ci::gl::TextureRef { return convert(bm); }) {
}

void TextureGifList::readerFinished() {
	mSurface = ci::Surface8u();
}

ci::gl::TextureRef TextureGifList::convert(const gif::Bitmap &bm) {
	if (bm.mPixels.empty()) return nullptr;
	// Error condition, should never happen
	if (bm.mPixels.size() != static_cast<size_t>(bm.mWidth*bm.mHeight)) throw std::runtime_error("Bitmap pixels do not match size");

	// Reuse a surface
	if (mSurface.getWidth() != bm.mWidth || mSurface.getHeight() != bm.mHeight) {
		mSurface = ci::Surface8u(bm.mWidth, bm.mHeight, true);
	}

	ci::Surface8u&		dest(mSurface);
	auto				src(bm.mPixels.begin());
	auto				pix = dest.getIter();
	while (pix.line()) {
		while (pix.pixel()) {
			pix.r() = src->r;
			pix.g() = src->g;
			pix.b() = src->b;
			pix.a() = 255;

			++src;
		}
	}
	ci::gl::Texture2d::Format		fmt;
	fmt.loadTopDown(true);
	auto ans = ci::gl::Texture2d::create(dest, fmt);
	glFlush();
	return ans;
}

} // namespace cs
