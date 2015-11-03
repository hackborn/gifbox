#ifndef APP_VIEW_FILENAVIGATIONVIEW_H_
#define APP_VIEW_FILENAVIGATIONVIEW_H_

#include <kt/view/button_view.h>
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

private:
	using base = kt::view::View;
	FileNavigationRef			mNavigation;
	kt::view::Button&			mPrevious;
	kt::view::Button&			mNext;
};

} // namespace cs

#endif
