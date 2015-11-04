#ifndef KT_VIEW_BUTTONVIEW_H_
#define KT_VIEW_BUTTONVIEW_H_

#include <kt/view/view.h>
#include "visual.h"

namespace kt {
namespace view {

/**
 * @class kt::view::Button
 * @brief Standard button view.
 * @description The button view class itself is mostly a shell, hosting a
 * button behaviour object to handle input, and a visual to display state.
 */
class Button : public kt::view::View {
public:
	// Pass-through convenience to v->addChildOrThrow
	static Button&				make(	View &parent, kt::Cns &cns)
										{ return parent.addChildOrThrow<Button>(new Button(cns)); }
	static Button&				make(	View &parent, kt::Cns &cns, const VisualRef &vis)
										{ Button& ans = parent.addChildOrThrow<Button>(new Button(cns)); ans.setVisual(vis); return ans; }
	Button() = delete;
	Button(const Button&) = delete;
	Button(kt::Cns&);

	void						setVisual(const VisualRef&);
	void						setVisualState(const std::string&) override;
	void						setClickFn(std::function<void(void)> fn) { mClickFn = fn; }

protected:
	void						onDraw(const kt::view::DrawParams&) override;

	void						onSizeChanged(const glm::vec3&) override;

private:
	using base = kt::view::View;
	VisualRef					mVisual;
	std::string					mLastVisualState;
	std::function<void(void)>	mClickFn;
};

} // namespace view
} // namespace kt

#endif
