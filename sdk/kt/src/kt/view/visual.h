#ifndef KT_VIEW_VISUAL_H_
#define KT_VIEW_VISUAL_H_

#include <string>
#include <cinder/Vector.h>
#include "view_defs.h"

namespace kt {
namespace view {
class Visual;
using VisualRef = std::shared_ptr<Visual>;

/**
 * @class kt::view::Visual
 * @brief Interface that abstracts drawable components.
 * @description Visuals pull drawing code out of the Views, to turn that into pluggable behaviour. Possibly
 * overkill -- the point of a view seems like it should be to draw -- but in practice views are often a
 * collection of drawing and behaviour, so this can be helpful.
 */
class Visual {
protected:
	Visual() { }

public:
	virtual ~Visual() { }

	// Metrics
	virtual glm::vec3			getPreferredSize() const { return glm::vec3(0.0f); }
	virtual glm::vec3			getSize() const { return glm::vec3(0.0f); }
	virtual void				setSize(const glm::vec3&) { }

	// Abstract interface for changing state. This works in conjunction with
	// behaviours to decouple state from visual.
	virtual void				setState(const std::string&) { }

	// Drawing function.
	virtual void				onDraw(const kt::view::DrawParams&) { }
};

} // namespace view
} // namespace kt

#endif
