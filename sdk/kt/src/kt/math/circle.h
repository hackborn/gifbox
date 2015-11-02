#ifndef KT_MATH_CIRCLE_H_
#define KT_MATH_CIRCLE_H_

#include <cinder/CinderMath.h>
#include <cinder/Vector.h>

namespace kt {
namespace math {

/**
 * @class kt::math::Circle2
 */
template <typename T>
class Circle2 {
public:
	using Vec2T = glm::tvec2<T, glm::defaultp>;

	Circle2();
	Circle2(const Vec2T &center, const T radius);
	Circle2(const Vec2T &center, const Vec2T &pt_on_radius);

	const Vec2T&			getCenter() const { return mCenter; }
	T						getRadius() const { return mRadius; }
	void					setRadius(const T r) { mRadius = r; }

	// Point on the edge of the circle, given the degree
	Vec2T					pointFromDegree(const T degree) const;
	// Get degree based on point on the edge. The radius is ignored.
	T						degreeFromPoint(const Vec2T&) const;

private:
	Vec2T					mCenter;
	T						mRadius;	
};

/**
 IMPLEMENTATION
 */
template<typename T>
Circle2<T>::Circle2()
		: mCenter(0.0f, 0.0f, 0.0f)
		, mRadius(0.0f) {
}

template<typename T>
Circle2<T>::Circle2(const Vec2T &center, const T radius)
		: mCenter(center)
		, mRadius(radius) {
}

template<typename T>
Circle2<T>::Circle2(const Vec2T &center, const Vec2T &pt_on_radius)
		: mCenter(center)
		, mRadius(center.distance(pt_on_radius)) {
}

template<typename T>
glm::tvec2<T, glm::defaultp> Circle2<T>::pointFromDegree(const T degree) const {
	return Vec2T(	mCenter.x + (mRadius * cos(cinder::toRadians(degree))),
					mCenter.y + (mRadius * sin(cinder::toRadians(degree))));
}

template<typename T>
T Circle2<T>::degreeFromPoint(const Vec2T &pt) const {
	const T				angle(static_cast<T>(atan2(pt.y-mCenter.y, pt.x-mCenter.x)));
	const T				deg(cinder::toDegrees(angle));
	if (deg < 0) return 360 + deg;
	return deg;
}

typedef Circle2<float>	Circle2f;
typedef Circle2<double>	Circle2d;

} // namespace math
} // namespace kt

#endif
