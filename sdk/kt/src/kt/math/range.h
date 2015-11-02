#ifndef KT_MATH_RANGE_H_
#define KT_MATH_RANGE_H_

#include <cstdint>

namespace kt {
namespace math {

/**
 * @class kt::math::Range
 * @brief Store a min and max and provide useful transformations between ranges.
 */
template <typename T>
class Range {
public:
	Range();
	Range(const T &min, const T &max);

	bool				operator==(const Range<T>&) const;
	bool				operator!=(const Range<T>&) const;

	T					getMin() const { return mMin; }
	T					getMax() const { return mMax; }

	void				set(const T _min, const T _max);
	
	bool				intersects(const Range<T>&) const;

	// Convert the value T in my range to the equivalent value in target range, answer that.
	// If v is outside of my range, the result will be outside of to_range.
	T					convert(const T &v, const Range<T> &to_range) const;
	// Answer a value that is always within my range, clipped if necessary.
	T					clip(const T&) const;
	// Answer the center value
	T					center() const;
	// Answer the value as a scaled unit, 0 - 1
	double				toUnit(const T&) const;

	T					mMin,
						mMax;
};

/**
 IMPLEMENTATION
 */
template<typename T>
Range<T>::Range()
		: mMin(0)
		, mMax(0) {
}

template<typename T>
Range<T>::Range(const T &min, const T &max)
		: mMin(min)
		, mMax(max) {
}

template<typename T>
bool Range<T>::operator==(const Range<T> &o) const {
	return mMin == o.mMin && mMax == o.mMax;
}

template<typename T>
bool Range<T>::operator!=(const Range<T> &o) const {
	return !(*this == o);
}

template<typename T>
void Range<T>::set(const T _min, const T _max) {
	mMin = _min;
	mMax = _max;
}

template<typename T>
bool Range<T>::intersects(const Range<T> &r) const {
	return !(mMax < r.mMin || mMin > r.mMax);
}

template<typename T>
T Range<T>::convert(const T &v, const Range<T> &to_range) const {
	if (mMax == mMin) return to_range.mMin;

	const T			from(mMax-mMin),
					to(to_range.mMax - to_range.mMin);
	return (((v-mMin) * to) / from) + to_range.mMin;
}

template<typename T>
T Range<T>::clip(const T &v) const {
	if (mMin <= mMax) {
		if (v <= mMin) return mMin;
		else if (v >= mMax) return mMax;
	} else {
		if (v <= mMax) return mMax;
		else if (v >= mMin) return mMin;
	}
	return v;
}

template<typename T>
T Range<T>::center() const {
	const T	half = static_cast<T>(0.5);
	return (mMin * half) + (mMax * half);
}

template <typename T>
double Range<T>::toUnit(const T &t) const {
	if (t <= mMin) return 0.0;
	if (t >= mMax) return 1.0;
	return static_cast<double>(t-mMin) / static_cast<double>(mMax-mMin);
}

typedef Range<float>		Rangef;
typedef Range<double>		Ranged;
typedef Range<std::int32_t>	Rangei;

extern const Rangef			RANGEF_UNI;		// 0.0 - 1.0
extern const Rangef			RANGEF_BI;		// -1.0 - 1.0

extern const Ranged			RANGED_UNI;		// 0.0 - 1.0
extern const Ranged			RANGED_BI;		// -1.0 - 1.0

} // namespace math
} // namespace kt

#endif
