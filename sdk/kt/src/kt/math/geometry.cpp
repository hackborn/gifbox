#include "geometry.h"

#include "segment.h"

namespace kt {
namespace math {

/**
 * @func new_point()
 */
void				new_point(const float x, const float y, const float m, const float r, float &x2, float &y2) {
	if (m == std::numeric_limits<float>::infinity() || m == -std::numeric_limits<float>::infinity()) {
		x2 = x;
		y2 = y + r;
		return;
	}

	const float		div = ci::math<float>::sqrt(1.0f + (m * m));
	const float		c = 1.0f / div,
					s = m / div;
	x2 = x + (r * c);
	y2 = y + (r * s);
}

/**
 * @func polygons_intersect()
 */
bool		polygons_intersect(const std::vector<glm::vec2> &a, const std::vector<glm::vec2>& b) {
	if (a.size() < 2 || b.size() < 2) return false;
	// Standard test -- see if any line segments intersect
	glm::vec2					last_a = a.back();
	for (const auto& pt_a : a) {
		const Segment2f			sega(last_a, pt_a);
		glm::vec2				last_b = b.back();
		for (const auto& pt_b : b) {
			const Segment2f		segb(last_b, pt_b);
			if (sega.intersects(segb)) return true;

			last_b = pt_b;
		}
		last_a = pt_a;
	}
	return true;
}

/**
 * @func scrub_polygon()
 */
void		scrub_polygon(std::vector<glm::vec2> &pts) {
	if (pts.size() < 2) return;

	std::vector<glm::vec2>		scratch;
	scrub_polygon(pts, scratch);
}

void		scrub_polygon(std::vector<glm::vec2> &pts, std::vector<glm::vec2> &scratch) {
	if (pts.size() < 2) return;

	scratch = pts;
	pts.clear();
	// Clearly not an exact science, but still, it reallly doesn't seem like
	// it should be this imprecise in order to start weeding out dupes.  Ugh.
	const float			EPS = 0.001f;
//	const float			EPS = std::numeric_limits<float>::min();
	for (const auto& pt : scratch) {
		if (pts.size() < 2) {
			pts.push_back(pt);
		} else {
			kt::math::Segment2f		seg(pts[pts.size()-2], pts[pts.size()-1]);
			seg.expand(10000.0f);
			if (seg.distanceToPoint(pt) <= EPS) {
				pts.back() = pt;
			} else {
				pts.push_back(pt);
			}
		}
	}

	if (pts.size() > 1 && pts.front() == pts.back()) pts.pop_back();
}

} // namespace math
} // namespace kt
