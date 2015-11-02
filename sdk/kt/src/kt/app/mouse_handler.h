#ifndef KT_APP_MOUSEHANDLER_H_
#define KT_APP_MOUSEHANDLER_H_

#include <cinder/Vector.h>
#include <cinder/app/FileDropEvent.h>
#include <cinder/app/MouseEvent.h>
//#include <rf/msg/msg_client.h>
#include <kt/view/view.h>
#include <kt/view/view_root.h>

namespace kt {

/**
 * @class kt::MouseHandler
 * @brief Handle translating mouse events and sending to a target view.
 * XXX Easiest implementation I could think of, lots of room for improvement.
 */
class MouseHandler {
public:
	MouseHandler() = delete;
	MouseHandler(const MouseHandler&) = delete;
	MouseHandler(kt::view::RootList &roots);

	void					clear();

	void					mouseDown(const ci::app::MouseEvent&);
	void					mouseMove(const ci::app::MouseEvent&);
	void					mouseUp(const ci::app::MouseEvent&);

	// File drop coordinates appear to be in desktop, but docs aren't clear on that.
//	void					fileDrop(	const ci::app::FileDropEvent&,
//										const ci::Vec2i &window_position);

private:
//	void					onMsg(const rf::msg::Msg&);
	kt::view::ViewRef		getHit(const ci::app::MouseEvent&, const std::uint32_t hit_flags);
	kt::view::ViewRef		getHit(const int x, const int y, const std::uint32_t hit_flags);

	kt::view::RootList&		mRoots;
//	kt::msg::Client			mMsgClient;
	// Find a view in mouse down, use that for subsequent move/up calls
	kt::view::ViewRef		mDownView;
	// Drag-drop sysem, which overrides normal pointer events.
//	kt::view::ViewRef		mDragStartView,
//							mLastDragView;
//	kt::view::DragDropData	mDragData;
};


} // namespace kt

#endif
