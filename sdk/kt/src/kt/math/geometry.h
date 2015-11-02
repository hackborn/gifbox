#ifndef KT_MATH_GEOMETRY_H_
#define KT_MATH_GEOMETRY_H_

#include <vector>
#include <cinder/CinderMath.h>
#include <cinder/Vector.h>

namespace kt {
namespace math {

/**
 * Add in some Cinder functions we lost in the move to glm
 */
inline glm::vec2	xy(const glm::vec3 &v) { return glm::vec2(v.x, v.y); }
inline float		lengthSquared(const glm::vec2 &v) { return v.x*v.x + v.y*v.y; }
inline float		lengthSquared(const glm::vec3 &v) { return v.x*v.x + v.y*v.y + v.z*v.z; }
inline float		distanceSquared(const glm::vec2 &a, const glm::vec2 &b) { return lengthSquared(a - b); }
inline float		distanceSquared(const glm::vec3 &a, const glm::vec3 &b) { return lengthSquared(a - b); }

/**
 * @func slope_2d()
 * @brief Find slope of a 2 dimensional line.
 */
inline float		slope_2d(const glm::vec2 &a, const glm::vec2 &b) { return (b.y-a.y) / (b.x-a.x); }
inline float		slope_2d(const float x1, const float y1, const float x2, const float y2) { return (y2-y1) / (x2-x1); }
template <typename T>
T					perpendicular_slope_2(const T x1, const T y1, const T x2, const T y2) { return -((x2-x1) / (y2-y1)); }
auto const perpendicular_slope_2f = &perpendicular_slope_2<float>;

/**
 * @func new_point()
 * @brief Answer a new point on a line.
 * @param x, y, m is the line in point-slope form
 * @param r is the distance from the point to place the new point
 */
void				new_point(const float x, const float y, const float m, const float r, float &x2, float &y2);

/**
 * @func polygons_intersect()
 * @brief Answer true if the polygons intersect.
 */
bool				polygons_intersect(const std::vector<glm::vec2> &a, const std::vector<glm::vec2>& b);

/**
 * @func scrub_polygon()
 * @brief Remove extranneous points (i.e. any point that is collinear to the
 * line segment of its previous and next points). Assume pts describe a single
 * valid polygon.
 */
void				scrub_polygon(std::vector<glm::vec2> &pts);
// Optionally provide the intermediate scratch vector (memory management issue only).
void				scrub_polygon(std::vector<glm::vec2> &pts, std::vector<glm::vec2> &scratch);

/**
 * @func			s_curve()
 * @brief Transform a 0-1 value to an s shape.
 */
template <typename T>
T					s_curve(const T v) { return (3.0f-2.0f*v)*v*v; }
auto const s_curvef = &s_curve<float>;
auto const s_curved = &s_curve<double>;

/**
 * @func			s_hump()
 * @brief Transform a 0-1 value to a hump (peak in the middle) where either sides
 * taper to 0 in an s curve.
 */
template <typename T>
T					s_hump(const T v) {
	if (v <= 0.5f) return s_curve<T>(v * 2.0f);
	return 1.0f - s_curve<T>((v - 0.5f) * 2.0f); }
auto const s_humpf = &s_hump<float>;
auto const s_humpd = &s_hump<double>;

} // namespace math
} // namespace kt

#endif
