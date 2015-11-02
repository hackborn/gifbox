#include "gif_algorithm.h"

#include <algorithm>
#include <unordered_map>

namespace gif {

namespace {
void		rgb_to_hsv(const gif::ColorA8u&, double &h, double &s, double &v);
}

/**
 * @class gif::BitmapToPaletteDefault
 * @brief Default implementation, currently simple clamp that finds the most-used colors.
 */
class BitmapToPaletteDefault : public BitmapToPalette {
public:
	BitmapToPaletteDefault() { }

	void			convert(const gif::Bitmap &src, const size_t max_size, gif::Palette &out) override {
		out.mColors.clear();
		if (src.empty()) return;

		// Simple utility to find all colors and eliminate based on a similarity until we're down to our max size.
		std::unordered_map<gif::ColorA8u, size_t>	ct;
		for (const auto& pix : src.mPixels) {
			const gif::ColorA8u		sc = gif::ColorA8u(pix.r, pix.g, pix.b, 255);
			ct[sc]++;
		}

		// For now, just clip based the most-used colors. CLEARLY THIS NEEDS TO CHANGE
		using Counter = std::pair<gif::ColorA8u, size_t>;
		std::vector<Counter>						vec;
		for (const auto& p : ct) vec.push_back(Counter(p.first, p.second));
		std::sort(vec.begin(), vec.end(), [](const Counter &a, const Counter &b)->bool{return a.second > b.second;});
		if (vec.size() > max_size) vec.resize(max_size);
		out.mColors.reserve(max_size);
		for (const auto& p : vec) out.mColors.push_back(p.first);
	}
};

BitmapToPaletteRef BitmapToPalette::create() {
	return std::make_shared<BitmapToPaletteDefault>();
}

/**
 * @class gif::ToColorIndexDefault
 * @brief Given a palette, find the nearest color match to each incoming color.
 * @description Convert an RGB palette to HSV values and find the nearest.
 */
class ToColorIndexDefault : public ToColorIndex {
public:
	ToColorIndexDefault() { }
	ToColorIndexDefault(const double weight_hue, const double weight_saturation, const double weight_value)
		: mWeightHue(weight_hue), mWeightSat(weight_saturation), mWeightVal(weight_value) { }

	void		setTo(const gif::Palette &pal) override {
		mColors.clear();
		for (const auto& c : pal.mColors) {
			HSV			hsv;
			rgb_to_hsv(c, hsv.h, hsv.s, hsv.v);
			mColors.push_back(Lookup(hsv, c));
		}
	}

	size_t		match(const gif::ColorA8u &c) const override {
		// Pulled from
		// https://social.msdn.microsoft.com/Forums/windows/en-us/105206d5-f7f7-4848-a32e-2b5cc10dc56f/how-to-find-the-nearest-matching-color-?forum=winforms
		// XXX Although not using that currently, I must have introduced a bug, simple RGB comparison right now.

		double			minDistance = std::numeric_limits<double>::max();
		size_t			index = 0, ci = 0;
		double			h, s, v;
		rgb_to_hsv(c, h, s, v);
		float			new_d = 255.0f * 4.0f;
		size_t			new_i = 0;
		for (const auto& p : mColors) {
	#if 1
			const float		dr = fabsf(static_cast<float>(c.r) - static_cast<float>(p.second.r));
			const float		dg = fabsf(static_cast<float>(c.g) - static_cast<float>(p.second.g));
			const float		db = fabsf(static_cast<float>(c.b) - static_cast<float>(p.second.b));
			const float		color_d = (dr + dg + db);
			if (color_d < new_d) {
				new_d = color_d;
				new_i = ci;
			}
	#else
			double		dH = p.first.h - h,
						dS = p.first.s - s,
						dV = p.first.v - v;
			double curDistance = std::sqrt(mWeightHue * std::pow(dH, 2) + mWeightSat * std::pow(dS, 2) + mWeightVal * std::pow(dV, 2));
			if (curDistance < minDistance) {
				minDistance = curDistance;
				index = ci;
			}
	#endif
			++ci;
		}
	//	return index;
		return new_i;
	}

private:
	struct HSV { double h = 0.0; double s = 0.0; double v = 0.0;};
	using Lookup = std::pair<HSV, gif::ColorA8u>;
	std::vector<Lookup>	mColors;

	double				mWeightHue = 0.8,
						mWeightSat = 0.1,
						mWeightVal = 0.1;
};

ToColorIndexRef ToColorIndex::create() {
	return std::make_shared<ToColorIndexDefault>();
}

/**
 * @class gif::ToPalettedBitmapDefault
 */
class ToPalettedBitmapDefault : public ToPalettedBitmap {
public:
	ToPalettedBitmapDefault() { }

	bool		convert(const gif::Bitmap &bm, const gif::ToColorIndexRef &tci, gif::PalettedBitmap &pbm) override {
		pbm.clear();
		if (bm.empty() || !tci) return false;
		pbm.setTo(bm.mWidth, bm.mHeight);
		if (bm.mPixels.size() != pbm.mPixels.size()) return false;

		auto		dst = pbm.mPixels.begin();
		for (const auto& c : bm.mPixels) {
			*dst = tci->match(c);
			++dst;
		}

		return true;
	}
};

ToPalettedBitmapRef ToPalettedBitmap::create() {
	return std::make_shared<ToPalettedBitmapDefault>();
}

namespace {

// Results in HSV of H=[0,360], S=[0,1], V=[0,1]
// if S == 0 then H = -1 (undefined)
void		rgb_to_hsv(const gif::ColorA8u &c, double &_h, double &_s, double &_v) {
	const double			r = static_cast<double>(c.r) / 255.0,
							g = static_cast<double>(c.g) / 255.0,
							b = static_cast<double>(c.b) / 255.0;
	const double			max = std::max<double>(r, std::max<double>(g, b));
	const double			min = std::min<double>(r, std::min<double>(g, b));
	const double			delta = max-min;
	_v = max;

	// gray
	if (min == max) {
		_h = -1.0;
		_s = 0.0;
		return;
	}

	if (max != 0.0) {
		_s = delta / max;
	} else {
		_s = 0.0;
		_h = -1;
		return;
	}

	if (r == max)			_h = (g-b) / delta;
	else if (g == max)		_h = 2 + (b-r) / delta;
	else					_h = 4 + (r-g) / delta;
	_h *= 60.0;
	if (_h < 0.0)			_h += 360;				
}

}

} // namespace gif
