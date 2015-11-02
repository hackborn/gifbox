#ifndef KT_MATH_SEGMENT_H_
#define KT_MATH_SEGMENT_H_

#include <cinder/CinderMath.h>
#include <cinder/PolyLine.h>
#include <cinder/Vector.h>

namespace kt {
namespace math {

/**
 * @class kt::math::Segment2f
 */
class Segment2f {
public:
	Segment2f();
	Segment2f(const float ax, const float ay, const float bx, const float by);
	Segment2f(const glm::vec2 &a, const glm::vec2 &b);

	const glm::vec2&	getA() const { return mA; }
	const glm::vec2&	getB() const { return mB; }
	void				set(const glm::vec2 &a, const glm::vec2 &b) { mA = a; mB = b;}

	float				slope() const;
	// Answer a slope, preventing infinite responses. Instead the
	// answer will be at the float limit.
	float				slopeNoInf() const;

	// Distance from segment to point
	float				distanceToPoint(const float ax, const float ay) const { return distanceToPoint(glm::vec2(ax, ay)); } 
	float				distanceToPoint(const glm::vec2&) const;
	// Distance from line to point
	float				lineDistanceToPoint(const float ax, const float ay) const { return lineDistanceToPoint(glm::vec2(ax, ay)); } 
	float				lineDistanceToPoint(const glm::vec2&) const;

	// Given a center point and a distance, find end points on this segment that are
	// distance from the center.
	void				findEnds(	const glm::vec2 &center, const float distance,
									glm::vec2 &a, glm::vec2 &b) const;

	// Answer true if the segments intersect, false otherwise.
	// If the lines are collinear, true is answered, and the intersection
	// is set to my start pt.
	bool				intersects(	const Segment2f&, glm::vec2 *intersection_pt = nullptr) const;
	bool				intersects(	const ci::PolyLine2f&, glm::vec2 *intersection_pt = nullptr,
									Segment2f *intersection_seg = nullptr) const;
	// A special method that finds the intersection segment most parallel to a supplied segment.
	bool				intersects(	const ci::PolyLine2f&, const Segment2f &parallel,
									glm::vec2 *intersection_pt = nullptr, Segment2f *intersection_seg = nullptr) const;

	// Grow either end by the given length
	void				expand(const float length);
	
private:
	glm::vec2			mA, mB;
};

} // namespace math
} // namespace kt

#endif
