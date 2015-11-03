#ifndef APP_VIEW_FILENAVIGATIONVIEW_H_
#define APP_VIEW_FILENAVIGATIONVIEW_H_

#include <kt/view/view.h>
#include "app/file_navigation.h"

namespace cs {

/**
 * @class cs::FileNavigationView
 * @brief Display the file navigation controls.
 */
class FileNavigationView : public kt::view::View {
public:
	FileNavigationView() = delete;
	FileNavigationView(const FileNavigationView&) = delete;
	FileNavigationView(kt::Cns&);

	void						setNavigation(FileNavigationRef);

	void						pointerDown(const kt::Pointer&) override;
	void						pointerUp(const kt::Pointer&) override;

protected:
//	void						onUpdate(const kt::UpdateParams&) override;
	void						onDraw(const kt::view::DrawParams&) override;

private:
	using base = kt::view::View;
//	kt::msg::Client				mMsgClient;
	FileNavigationRef			mNavigation;
};

} // namespace cs

#endif
