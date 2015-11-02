#ifndef GIFWRAP_GIFALGORITHM_H_
#define GIFWRAP_GIFALGORITHM_H_

#include <memory>
#include "gif_bitmap.h"

/**
 * Collection of abstract objects that can be plugged into the framework.
 * This is intendend to localize all subjective algorithms, i.e. anything
 * that might vary based on context or perception, from all deterministic
 * algorithms, which will only be right or wrong.
 */

namespace gif {
class BitmapToPalette;
using BitmapToPaletteRef = std::shared_ptr<BitmapToPalette>;
class ToColorIndex;
using ToColorIndexRef = std::shared_ptr<ToColorIndex>;
class ToPalettedBitmap;
using ToPalettedBitmapRef = std::shared_ptr<ToPalettedBitmap>;

/**
 * @class gif::BitmapToPalette
 * @brief Convert an RGBA bitmap to a palette.
 */
class BitmapToPalette {
protected:
	BitmapToPalette() { }
public:
	virtual ~BitmapToPalette() { }

	// @param max_size is the maximum allowed size of the final palette.
	virtual void				convert(const gif::Bitmap&, const size_t max_size, gif::Palette&) = 0;

	// Implementations
	static BitmapToPaletteRef	create();
};

/**
 * @class gif::ToColorIndex
 * @brief Given a palette, find the nearest color match to each incoming color.
 */
class ToColorIndex {
protected:
	ToColorIndex() { }
public:
	virtual ~ToColorIndex() { }

	virtual void				setTo(const gif::Palette&) = 0;
	virtual size_t				match(const gif::ColorA8u&) const = 0;

	// Implementations
	static ToColorIndexRef		create();
};

/**
 * @class gif::ToPalettedBitmap
 * @brief Convert an RGBA bitmap to a paletted bitmap.
 */
class ToPalettedBitmap {
protected:
	ToPalettedBitmap() { }
public:
	virtual ~ToPalettedBitmap() { }

	virtual bool				convert(const gif::Bitmap&, const gif::ToColorIndexRef&, gif::PalettedBitmap &pbm) = 0;

	// Implementations
	static ToPalettedBitmapRef	create();
};

} // namespace gif

#endif
