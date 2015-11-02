#ifndef GIFWRAP_GIFBITMAP_H_
#define GIFWRAP_GIFBITMAP_H_

#include <vector>
#include "gif_color.h"

namespace gif {

/**
 * @class gif::Bitmap
 * @brief A local bitmap definition, an array of colours.
 */
class Bitmap {
public:
	Bitmap() { }
	Bitmap(const int32_t w, const int32_t h) { setTo(w, h); }

	bool						empty() const { return mWidth < 1 || mHeight < 1; }
	void						setTo(const int32_t w, const int32_t h) {
		if (w == mWidth && h == mHeight) return;
		mWidth = w;
		mHeight = h;
		mPixels.clear();
		if (w > 0 && h > 0) mPixels.resize(w * h);
	}

	int32_t						mWidth = 0,
								mHeight = 0;
	std::vector<gif::ColorA8u>	mPixels;
};

/**
 * @class gif::PalettedBitmap
 * @brief A bitmap converted to palette codes.
 */
class PalettedBitmap {
public:
	PalettedBitmap() { }
	PalettedBitmap(const int32_t w, const int32_t h) { setTo(w, h); }

	bool						empty() const { return mWidth < 1 || mHeight < 1; }

	void						clear() {
		mWidth = mHeight = 0;
		mPixels.clear();
	}

	void						setTo(const int32_t w, const int32_t h) {
		if (w == mWidth && h == mHeight) return;
		mWidth = w;
		mHeight = h;
		mPixels.clear();
		if (w > 0 && h > 0) mPixels.resize(w * h);
	}

	int32_t						mWidth = 0,
								mHeight = 0;
	std::vector<uint8_t>		mPixels;
};

} // namespace gif

#endif
