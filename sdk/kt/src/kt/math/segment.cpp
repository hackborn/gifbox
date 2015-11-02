#include "segment.h"

#include "geometry.h"

namespace kt {
namespace math {

/**
 * @class kt::math::Segment2f
 */
Segment2f::Segment2f()
		: mA(glm::vec2(0.0f, 0.0f))
		, mB(glm::vec2(0.0f, 0.0f)) {
}

Segment2f::Segment2f(const float ax, const float ay, const float bx, const float by)
		: mA(ax, ay)
		, mB(bx, by) {
}

Segment2f::Segment2f(const glm::vec2 &a, const glm::vec2 &b)
		: mA(a)
		, mB(b) {
}

float Segment2f::slope() const {
	// Why was I doing this? I get that it was removing inf as a response, but
	// some clients would need to know that. And also, it was just wrong.
//	if (mB.x == mA.x) return 1.0f;
//	if (mB.y == mA.y) return 0.0f;
	return (mB.y-mA.y) / (mB.x-mA.x);
}

float Segment2f::slopeNoInf() const {
	if (mB.x == mA.x) return std::numeric_limits<float>::max();
	return (mB.y-mA.y) / (mB.x-mA.x);
}

float Segment2f::distanceToPoint(const glm::vec2 &pt) const {
	// From stackoverflow:
	// http://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
	const float			l2(distanceSquared(mA, mB));
	if (l2 == 0.0f) return glm::distance(pt, mA);

	// Consider the line extending the segment, parameterized as v + t (w - v).
	// We find projection of point p onto the line. 
	// It falls where t = [(pt-mA) . (mB-mA)] / |mB-mA|^2
	const float t = glm::dot(pt-mA, mB-mA) / l2;
	if (t < 0.0) return glm::distance(pt, mA);		// Beyond the 'v' end of the segment
	else if (t > 1.0) return glm::distance(pt, mB);	// Beyond the 'w' end of the segment
	const auto projection = mA + t * (mB - mA);	// Projection falls on the segment
	return glm::distance(pt, projection);
}

float Segment2f::lineDistanceToPoint(const glm::vec2 &pt) const {
// XXX Doesn't really seem to work, though

	// From stackoverflow:
	// http://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
	// (answer by Dmitry, way down the page)

	float ch = (mA.y - mB.y) * pt.x + (mB.x - mA.x) * pt.y + (mA.x * mB.y - mB.x * mA.y);
	float del = ci::math<float>::sqrt(ci::math<float>::pow(mB.x - mA.x, 2.0f) + ci::math<float>::pow(mB.y - mA.y, 2.0f));
	float d = ch / del;
	return d;
}

void Segment2f::findEnds(	const glm::vec2 &center, const float distance,
							glm::vec2 &a, glm::vec2 &b) const {
	// Need a slope for this to make sense.
	if (mA == mB) {
		a = center;
		b = center;
		return;
	}
	auto		vec = mB-mA;
	glm::normalize(vec);
	vec *= distance;
	a = center - vec;
	b = center + vec;
}

bool Segment2f::intersects(const Segment2f &b, glm::vec2 *intersection_pt) const {
	// From stackoverflow:
	// http://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect

    float s02_x, s02_y, s10_x, s10_y, s32_x, s32_y, s_numer, t_numer, denom, t;
    s10_x = mB.x - mA.x;
    s10_y = mB.y - mA.y;
    s32_x = b.mB.x - b.mA.x;
    s32_y = b.mB.y - b.mA.y;

    denom = s10_x * s32_y - s32_x * s10_y;
    if (denom == 0) {
		if (intersection_pt) *intersection_pt = mA;
        return true; // Collinear
	}
    bool denomPositive = denom > 0;

    s02_x = mA.x - b.mA.x;
    s02_y = mA.y - b.mA.y;
    s_numer = s10_x * s02_y - s10_y * s02_x;
    if ((s_numer < 0) == denomPositive)
        return false; // No collision

    t_numer = s32_x * s02_y - s32_y * s02_x;
    if ((t_numer < 0) == denomPositive)
        return false; // No collision

    if (((s_numer > denom) == denomPositive) || ((t_numer > denom) == denomPositive))
        return false; // No collision

    // Collision detected
    t = t_numer / denom;
	if (intersection_pt) {
        intersection_pt->x = mA.x + (t * s10_x);
        intersection_pt->y = mA.y + (t * s10_y);
	}
    return true;
}

bool Segment2f::intersects(	const ci::PolyLine2f &b, glm::vec2 *intersection_pt,
							Segment2f *intersection_seg) const {
	const auto& pts = b.getPoints();
	if (pts.size() < 2) return false;

	glm::vec2		prevpt = pts.back();
	for (const auto& nextpt : pts) {
		if (intersects(Segment2f(prevpt, nextpt), intersection_pt)) {
			if (intersection_seg) intersection_seg->set(prevpt, nextpt);
			return true;
		}
		prevpt = nextpt;
	}
	return false;
}

namespace {

// Answer true if A slope is closer to M than B.
bool		slope_is_closer(const float m, const float a, const float b) {
	// Should we ignore negatives here or not?
	const float		dista = fabsf(m) - fabsf(a),
					distb = fabsf(m) - fabsf(b);
	return dista < distb;
}

}

bool Segment2f::intersects(	const ci::PolyLine2f &b, const Segment2f &parallel,
							glm::vec2 *intersection_pt, Segment2f *intersection_seg) const {
	const auto& pts = b.getPoints();
	if (pts.size() < 2) return false;

	glm::vec2				prevpt = pts.back();
	const float				my_slope = parallel.slopeNoInf();
	float					target_m = 0.0f;
	glm::vec2				target_pt;
	Segment2f				target_seg;
	bool					found = false;
	for (const auto& nextpt : pts) {
		const Segment2f		iseg(prevpt, nextpt);
		glm::vec2			ipt;
		if (intersects(iseg, &ipt)) {
			if (found) {
				const float	m = iseg.slopeNoInf();
				if (my_slope == m || slope_is_closer(my_slope, m, target_m)) {
					target_m = m;
					target_pt = ipt;
					target_seg = iseg;
				} else {
					
				}
			} else {
				found = true;
				target_m = iseg.slopeNoInf();
				target_pt = ipt;
				target_seg = iseg;
			}
		}
		prevpt = nextpt;
	}
	if (found) {
		if (intersection_pt) *intersection_pt = target_pt;
		if (intersection_seg) *intersection_seg = target_seg;
		return true;
	}
	return false;
}

void Segment2f::expand(const float length) {
	// Can't grow a point, can we?
	if (mA == mB) return;

	auto		vec = mB-mA;
	glm::normalize(vec);
	vec *= length;
	mA -= vec;
	mB += vec;
}

} // namespace math
} // namespace kt
