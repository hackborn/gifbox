#include "mouse_handler.h"

#include <kt/view/view.h>
//#include <rf/view/private/drag_drop_support.h>

namespace kt {

namespace {
const std::int32_t			MOUSE_POINTER_ID(-100);
Pointer						make_pointer(const std::int32_t id, const ci::app::MouseEvent&, const kt::view::ViewRef&);
glm::vec3					local_pt(const kt::view::ViewRef&, const ci::app::MouseEvent&);
glm::vec3					local_pt(const kt::view::ViewRef&, const int x, const int y);
}

/**
 * @class kt::MouseHandler
 */
MouseHandler::MouseHandler(kt::view::RootList &roots)
		: mRoots(roots) {
//		, mMsgClient(root_view.getCns(), [this](const rf::msg::Msg &m) { onMsg(m); }) {
//	mMsgClient.add(kt::view::DragStartMsg::CHANNEL());
}

void MouseHandler::clear() {
	mDownView.reset();
//	mDragStartView.reset();
//	mLastDragView.reset();
}

void MouseHandler::mouseDown(const ci::app::MouseEvent &e) {
	mDownView = getHit(e, kt::view::HIT_POINTER_EVENTS);
	if (mDownView) {
		mDownView->pointerDown(make_pointer(MOUSE_POINTER_ID, e, mDownView));
	}
}

void MouseHandler::mouseMove(const ci::app::MouseEvent &e) {
#if 0
	// Override the normal events if there's a drag happening.
	if (mDragStartView) {
		rf::view::ViewRef		next_v(getHit(e, rf::view::HIT_DRAG_EVENTS));
		if (mLastDragView == next_v) {
			if (mLastDragView) mLastDragView->onMsg(rf::view::DragOverMsg(rf::view::DragOverMsg::Stage::kOver, mDragData, local_pt(mLastDragView, e)));
		} else {
			if (mLastDragView) mLastDragView->onMsg(rf::view::DragOverMsg(rf::view::DragOverMsg::Stage::kExited, mDragData, local_pt(mLastDragView, e)));
			mLastDragView = next_v;
			if (mLastDragView) mLastDragView->onMsg(rf::view::DragOverMsg(rf::view::DragOverMsg::Stage::kEntered, mDragData, local_pt(mLastDragView, e)));
		}
		return;
	}
#endif

	if (mDownView) {
		const glm::vec3			global_pt(static_cast<float>(e.getX()), static_cast<float>(e.getY()), 0.0f);
		const bool				is_inside = mDownView->contains(global_pt);
		std::uint32_t			flags(0);
		if (is_inside) flags |= kt::view::View::INSIDE_DOWN_VIEW;
		mDownView->pointerMoved(make_pointer(MOUSE_POINTER_ID, e, mDownView), flags);
	}
}

void MouseHandler::mouseUp(const ci::app::MouseEvent &e) {
#if 0
	// Override the normal events if there's a drag happening.
	if (mDragStartView) {
		if (mLastDragView) mLastDragView->onMsg(rf::view::DragDropMsg(mDragData, local_pt(mLastDragView, e)));
		mDragStartView->onMsg(rf::view::DragCompleteMsg());
		mDragStartView.reset();
		mLastDragView.reset();
		return;
	}
#endif

	if (mDownView) {
		mDownView->pointerUp(make_pointer(MOUSE_POINTER_ID, e, mDownView));
		mDownView.reset();
	}
}

#if 0
void MouseHandler::fileDrop(const ci::app::FileDropEvent &d, const ci::Vec2i &window_position) {
	const int				x(window_position.x + d.getX()),
							y(window_position.y + d.getY());
	kt::view::ViewRef		v{getHit(x, y, kt::view::HIT_DRAG_EVENTS)};
	if (v) {
		v->onMsg(rf::view::FileDropMsg(d, local_pt(v, x, y)));
	}
}
#endif

#if 0
void MouseHandler::onMsg(const kt::msg::Msg &_m) {
	if (rf::view::DragStartMsg::WHAT() == _m.mWhat) {
		const rf::view::DragStartMsg&		m((const rf::view::DragStartMsg&)_m);
		mDragStartView = m.mView;
		mLastDragView.reset();
		mDragData = m.mData;
		// Take over from the down event.
		mDownView.reset();
	}
}
#endif

kt::view::ViewRef MouseHandler::getHit(const ci::app::MouseEvent &e, const std::uint32_t hit_flags) {
	return getHit(e.getX(), e.getY(), hit_flags);
}

kt::view::ViewRef MouseHandler::getHit(const int x, const int y, const std::uint32_t hit_flags) {
	const glm::vec3		global_pt(	static_cast<float>(x),
									static_cast<float>(y),
									0.0f);
	// We run this in reverse because we default to treating things like 2d layers.
	for (auto it=mRoots.rbegin(), end=mRoots.rend(); it!=end; ++it) {
		if (!(*it)) continue;

		kt::view::ViewRef	ans = (*it)->getHit(global_pt, hit_flags);
		if (ans) return ans;
	}
	return nullptr;
}

namespace {

Pointer						make_pointer(const std::int32_t id, const ci::app::MouseEvent &e, const kt::view::ViewRef &v) {
	Pointer					ans;
	const glm::vec3			local_pt(v->localFromGlobal(glm::vec3(static_cast<float>(e.getX()), static_cast<float>(e.getY()), 0.0f)));
	ans.mId = id;
	ans.mX = local_pt.x;
	ans.mY = local_pt.y;
	if (e.isLeftDown()) ans.mButtons |= (1<<0);
	if (e.isRightDown()) ans.mButtons |= (1<<1);
	if (e.isMiddleDown()) ans.mButtons |= (1<<2);
	return ans;
}

glm::vec3		local_pt(const kt::view::ViewRef &v, const ci::app::MouseEvent &e) {
	return local_pt(v, e.getX(), e.getY());
}

glm::vec3		local_pt(const kt::view::ViewRef &v, const int x, const int y) {
	const glm::vec3		global_pt(	static_cast<float>(x),
									static_cast<float>(y),
									0.0f);
	if (v) return v->localFromGlobal(global_pt);
	return global_pt;
}

}

} // namespace kt
