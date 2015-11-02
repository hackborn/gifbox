#ifndef KT_APP_APP_H_
#define KT_APP_APP_H_

#include <cinder/app/App.h>
#include <kt/view/view_root.h>
#include "cns.h"
#include "mouse_handler.h"
#include "update.h"

namespace kt {
class App;
class DirectoryWatcher;

/**
 * @class kt::App
 * @brief Superclass for applications.
 */
class App : public ci::app::App {
public:
	App();
	virtual ~App();

	void					update() override;
	void					draw() override;

	static void				prepareKtSettings(Settings*);
	void					setup() override;
	void					cleanup() override;

	void					mouseDown(ci::app::MouseEvent) override;
	// Don't really like this but drag is called when a button is down, otherwise move is called
	void					mouseDrag(ci::app::MouseEvent) override;
	void					mouseMove(ci::app::MouseEvent) override;
	void					mouseUp(ci::app::MouseEvent) override;

protected:
	// Only valid in update() after calling the parent update()
	const kt::UpdateParams&	getUpdateParams() const { return mUpdateParams; }

	kt::view::PerspRoot&	addPerspRoot(const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0));
	kt::view::OrthoRoot&	addOrthoRoot();

	kt::Cns					mCns;

private:
	void					settingsDirectoryChanged(const std::string &path);

	using base = ci::app::App;
	kt::UpdateParams		mUpdateParams;
	kt::app_time			mLastTime;
	kt::UpdateList			mUpdateList;
	kt::view::RootList		mRoots;
	MouseHandler			mMouseHandler;
	// If the settings are live, create a client to handle that.
	// Needs to be done this way primarily to control startup order.
	std::unique_ptr<DirectoryWatcher>
							mSettingsWatcher;
	bool					mSettingsAreLive = false;
};

} // namespace kt

#endif
