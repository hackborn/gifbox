#ifndef GIFWRAP_GIFCOLOR_H_
#define GIFWRAP_GIFCOLOR_H_

#include <cstdint>
#include <vector>

namespace gif {

/**
 * @class gif::ColorA8u
 * @brief A local color defintion, RGBA with 8 bits per pixel.
 */
struct ColorA8u {
	ColorA8u() { }
	ColorA8u(const uint8_t _r, const uint8_t _g, const uint8_t _b, const uint8_t _a = 255) : r(_r), g(_g), b(_b), a(_a) { }

	bool		operator==(const ColorA8u &c) const { return r == c.r && g == c.g && b == c.b && a == c.a; }

	uint8_t		r = 0,
				g = 0,
				b = 0,
				a = 0;
};

/**
 * @class gif::Palette
 * @brief A color table.
 */
struct Palette {
	Palette() { }

	bool						empty() const { return mColors.empty(); }
	size_t						size() const { return mColors.size(); }

	// Color tables need to be a power of 2, so expand or clip if needed
	void						clip(const size_t max_size = 1<<8) {
		// Clip
		if (mColors.size() >= max_size) {
			mColors.resize(max_size);
			return;
		}
		// Expand
		size_t					last_size = max_size;
		for (size_t bits=8; bits>=2; --bits) {
			const size_t		size = 1<<bits;
			if (mColors.size() >= size) {
				mColors.resize(last_size);
				return;
			}
			last_size = size;
		}
	}

	std::vector<gif::ColorA8u>	mColors;
};

} // namespace gif

namespace std {
template<>
struct hash<gif::ColorA8u> : public unary_function<gif::ColorA8u, size_t> {
	std::size_t operator()(const gif::ColorA8u& c) const {
		return	(static_cast<size_t>(c.r) << 24)
				 | (static_cast<size_t>(c.g) << 16)
				 | (static_cast<size_t>(c.b) << 8)
				 | (static_cast<size_t>(c.a) << 0);
	}
};

}
#endif
