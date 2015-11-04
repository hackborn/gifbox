#ifndef KT_BEHAVIOR_BUTTONBEHAVIOR_H_
#define KT_BEHAVIOR_BUTTONBEHAVIOR_H_

#include <functional>
#include "behavior.h"

namespace kt {
class ButtonBehavior;
using ButtonBehaviorRef = std::shared_ptr<ButtonBehavior>;

/**
 * @class kt::ButtonBehavior
 * @brief Translate pointer events to button behavior.
 */
class ButtonBehavior : public Behavior {
public:
	ButtonBehavior();

	// Called when the visual state should change
	// Will send these view states: REST_STATE, DOWN_STATE.
	void						setVisualStateFn(std::function<void(const std::string&)> fn) { mVisualStateFn = fn; }
	// Called when the button is clicked.
	void						setClickFn(std::function<void(void)> fn) { mClickFn = fn; }

	void						pointerDown(const Pointer&) override;
	void						pointerMoved(const Pointer&, const std::uint32_t flags = 0) override;
	void						pointerUp(const Pointer&) override;

private:
	std::function<void(const std::string&)>
								mVisualStateFn;
	std::function<void(void)>	mClickFn;
};

} // namespace kt

#endif
