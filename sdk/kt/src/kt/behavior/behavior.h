#ifndef KT_BEHAVIOR_BEHAVIOR_H_
#define KT_BEHAVIOR_BEHAVIOR_H_

#include <memory>
#include <kt/app/pointer.h>

namespace kt {
class Behavior;
using BehaviorRef = std::shared_ptr<Behavior>;

/**
 * @class kt::Behavior
 * @brief Abstract superclass for objects that consume pointer events and translate them to actions.
 */
class Behavior {
protected:
	Behavior() { }
public:
	virtual ~Behavior() { }

	virtual void				pointerDown(const Pointer&) = 0;
	virtual void				pointerMoved(const Pointer&, const std::uint32_t flags = 0) = 0;
	virtual void				pointerUp(const Pointer&) = 0;
};

} // namespace kt

#endif
