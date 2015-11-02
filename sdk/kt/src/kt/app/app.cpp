#include "app.h"

#include <cinder/gl/scoped.h>
#include <kt/service/directory_watcher_service.h>
#include <kt/service/font_service.h>
#include <kt/service/update_service.h>
#include "directory_watcher.h"
#include "environment.h"

namespace kt {

/**
 * @class kt::App
 */
App::App()
		: mMouseHandler(mRoots) {
	// System services (that are not reliant on any GL or other setup)
	// UpdateService should be the first thing created, since so many
	// other things rely on it.
	mCns.getService<kt::UpdateService>([this]()->kt::UpdateService*{return new kt::UpdateService(mUpdateList);});

	// Start the settings watcher if that's enabled.
	// Otherwise, it still needs to be created, so it exists for clients,
	// but we don't bother to start it.
	// XXX It'd be nice to have this a little cleaner -- to create a completely empty
	// dummy service if the app doesn't need it at all.
	DirectoryWatcherService&	dws(mCns.getService<kt::DirectoryWatcherService>([this]()->kt::DirectoryWatcherService*{ return new kt::DirectoryWatcherService(mCns); }));
	if (mCns.getSettings("system").getBool("app:live_settings") && !mCns.mSettingsDirectories.empty()) {
		try {
			for (const auto& path : mCns.mSettingsDirectories) {
				dws.addPath(path);
			}
			dws.start();
			mSettingsWatcher.reset(new DirectoryWatcher(mCns, [this](const std::string &p) { settingsDirectoryChanged(p);}));
			mSettingsAreLive = true;
		} catch (std::exception const&) {
		}
	}
}

App::~App() {
	mSettingsWatcher.reset();
	mMouseHandler.clear();
	mRoots.clear();
	// Remove the updateservice and keep it around, to make sure it
	// is the last service released, since others might rely on it.
	std::unique_ptr<Service>				update_service(std::move(mCns.mServices.find(kt::UpdateService::NAME())->second));
	mCns.mServices.clear();
}

void App::update() {
	kt::UpdateParams&				up(mUpdateParams);
	up.mTime = kt::UpdateParams().mTime;
	if (mLastTime != kt::app_time()) up.mLastTime = mLastTime;
	std::chrono::duration<double>	elapsed = up.mTime - up.mLastTime;
	up.mElapsed = elapsed.count();
	mLastTime = up.mTime;

	mUpdateList.update(up);

	for (auto& it : mRoots) {
		if (it) it->update(up);
	}
}

void App::draw() {
	kt::view::DrawParams		p;

	ci::gl::clear(ci::Color( 0.1f, 0.1f, 0.15f ));
	ci::gl::ScopedBlendAlpha blendScope;

	for (auto& it : mRoots) {
		if (it) it->draw(p);
	}
}

void App::prepareKtSettings(Settings*) {
}

void App::setup() {
	// System services
	mCns.getService<kt::FontService>([]()->kt::FontService*{return new kt::FontService();});

	const glm::vec2			window_size(static_cast<float>(getWindowWidth()), static_cast<float>(getWindowHeight()));
	const ci::Area			area(getWindowBounds());
	for (auto& r : mRoots) {
		if (!r) continue;
		r->setViewport(area);
		// Default orthos to the full screen
		kt::view::OrthoRoot*	ortho = dynamic_cast<kt::view::OrthoRoot*>(r.get());
		if (ortho) ortho->setCameraOrtho(0.0f, window_size.x, window_size.y, 0.0f, -1.0f, 1.0f);
	}

	// Startup report
	{
		std::cout << "Startup" << std::endl;
		std::cout << "\tlocal=" << mCns.getSettings("system").getString("app:local") << std::endl;
		if (mSettingsAreLive) {
			std::cout << "\tlive_settings=on" << std::endl;
		}
	}
}

void App::cleanup() {
	for (auto& s : mCns.mServices) {
		if (s.second) s.second->shutdown();
	}
}

void App::mouseDown(ci::app::MouseEvent e) {
	mMouseHandler.mouseDown(e);
}

void App::mouseDrag(ci::app::MouseEvent e) {
	mMouseHandler.mouseMove(e);
}

void App::mouseMove(ci::app::MouseEvent e) {
	mMouseHandler.mouseMove(e);
}

void App::mouseUp(ci::app::MouseEvent e) {
	mMouseHandler.mouseUp(e);
}

kt::view::PerspRoot& App::addPerspRoot(const glm::vec3 &up) {
	std::shared_ptr<kt::view::PerspRoot>	r(new kt::view::PerspRoot(mCns, up));
	if (!r) throw std::runtime_error("Can't add perspective root");
	mRoots.push_back(r);
	return *(r.get());
}

kt::view::OrthoRoot& App::addOrthoRoot() {
	std::shared_ptr<kt::view::OrthoRoot>	r(new kt::view::OrthoRoot(mCns));
	if (!r) throw std::runtime_error("Can't add orthogonal root");
	mRoots.push_back(r);
	return *(r.get());
}

void App::settingsDirectoryChanged(const std::string &path) {
	mCns.updateSettings(path);
}

} // namespace kt
