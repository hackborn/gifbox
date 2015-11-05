#include "app.h"

#include <cinder/app/RendererGl.h>
#include <cinder/gl/gl.h>
#include <cinder/Filesystem.h>
#include <kt/app/environment.h>
#include <kt/app/kt_string.h>
#include <gifwrap/gif_file.h>
#include "cinder_gif/path_writer.h"
#include "cinder_gif/texture_gif_list.h"
#include "msg/app_msg.h"
#include "view/status_view.h"

namespace cs {

namespace {
// True if the path is a GIF.
bool						is_gif(const ci::fs::path&);
// True if the path is a non-GIF image (jpg, png).
bool						is_image(const ci::fs::path&);
// Get the filename from the path.
std::string					get_filename(std::string path);
}

App::App()
		: mMsgClient(mCns)
		, mRoot(addOrthoRoot())
		, mHudView(mRoot.addChildOrThrow<kt::view::View>(new kt::view::View(mCns)))
		, mMediaView(mHudView.addChildOrThrow<MediaView>(new MediaView(mCns)))
		, mGifView(mMediaView.setMediaViewOrThrow<GifView>(new GifView(mCns)))
		, mFileNavigationView(mHudView.addChildOrThrow<FileNavigationView>(new FileNavigationView(mCns)))
		, mStatusView(mHudView.addChildOrThrow<StatusView>(new StatusView(mCns))) {
	mMsgClient.add<SetMediaPathMsg>([this](const SetMediaPathMsg &m) {onSetMediaPath(m);});
}

App::~App() {
	mQuit = true;
	try {
		mThread.join();
	} catch (std::exception const&) {
	}
}

void App::prepareSettings(Settings* s) {
	if (s) {
		s->setTitle("gifbox");
		s->setWindowSize(glm::ivec2(1920, 1080));
//		s->setFullScreen(true);
//		s->setConsoleWindowEnabled(true);
	}
}

void App::setup() {
	base::setup();

	// Layout the views
	mFileNavigationView.setCenter(0.0f, 1.0f);
	mStatusView.setCenter(1.0f, 1.0f);
	layout();

	mParams = ci::params::InterfaceGl::create("Params", glm::ivec2(220, 120));
	mParams->addParam("Frame",	&mFrame, "", true);
	mParams->addParam<float>("Speed", &mPlaybackSpeed, false).min(0).max(8).step(0.01f).precision(2);
	mParams->addParam<float>("Size", &mGifSize, false).min(0).max(1).step(0.01f).precision(2);

	// Start a thread to handle the actual loading
	mQuit = false;
	ci::gl::ContextRef backgroundCtx = ci::gl::Context::create(ci::gl::context());
	mThread = std::thread( bind( &App::gifThread, this, backgroundCtx));

	// Load the default GIF.
	auto	input = mThreadInput.make();
	input->mPaths.push_back(kt::env::expand("$(DATA)/tumblr_n8njbcmeWS1t9jwm6o1_400.gif"));
	input->mReplaceNavigation = true;
	mThreadInput.push(input);
}

void App::resize() {
	base::resize();
	layout();
}

void App::keyDown(ci::app::KeyEvent e) {
	if( e.getChar() == 'f' ) {
		// Toggle full screen when the user presses the 'f' key.
		setFullScreen( ! isFullScreen() );
	} else if( e.getCode() == ci::app::KeyEvent::KEY_ESCAPE ) {
		// Exit full screen, or quit the application, when the user presses the ESC key.
		if( isFullScreen() )
			setFullScreen( false );
		else
			quit();
	}
}

void App::fileDrop(ci::app::FileDropEvent e) {
	try {
		auto				input(makeInput(e));
		if (!input) return;

		if (input->mType == InputType::kSave) {
			input->mSavePath = getSaveFilePath().string();
			if (input->mSavePath.empty()) return;
		}
		input->mReplaceNavigation = true;
		mThreadInput.push(input);
	} catch (std::exception const&) {
	}
}

void App::update() {
	// Get current GIF list
	auto		list = mThreadOutput.pop();
	if (list) {
		mGifView.setTextures(list->mGifList);
		if (list->mReplaceNavigation) {
			if (list->mPaths.size() == 1) {
				mFileNavigationView.setNavigation(std::make_shared<DirectoryNavigation>(list->mPaths.front()));
			} else {
				mFileNavigationView.setNavigation(std::make_shared<FileListNavigation>(list->mPaths));
			}
		}
	}

	// Update the view
	mGifView.setPlaybackSpeed(mPlaybackSpeed);
	mMediaView.setMediaSize(mGifSize);
	base::update();

	// Update params
	mFrame = static_cast<int32_t>(mGifView.getCurrentFrame());
	mParams->hide();	// Params don't seem designed for a high update rate, this flushes it.
	mParams->show();

	// Update status
	mStatusTransport.pop_all(mStatus);
	for (const auto& it : mStatus) StatusMsg(it).send(mCns);
	mStatus.clear();
}

void App::draw() {
	base::draw();
	mParams->draw();
}

void App::layout() {
	const glm::ivec2	iwin_size(getWindowWidth(), getWindowHeight());
	const glm::vec2		win_size(static_cast<float>(iwin_size.x), static_cast<float>(iwin_size.y));
	mHudView.setSize(win_size);
	mMediaView.setSize(win_size);
	mFileNavigationView.setPosition(10.0f, floorf(win_size.y-10.0f));
	mStatusView.setPosition(floorf(win_size.x-10.0f), floorf(win_size.y-10.0f));
}

void App::onSetMediaPath(const SetMediaPathMsg &m) {
	if (!m.mPath.empty()) {
		auto	input = mThreadInput.make();
		input->mPaths.push_back(m.mPath);
		mThreadInput.push(input);
	}
}

std::shared_ptr<App::Input> App::makeInput(const ci::app::FileDropEvent &e) const {
	auto				input = mThreadInput.make();
	if (!input) return nullptr;
	for (const auto& it : e.getFiles()) {
		// Only add gifs to load input, images to save input. Take the first of whatever I find
		if (is_gif(it)) {
			if (input->mPaths.empty() || input->mType == InputType::kLoad) {
				input->mType = InputType::kLoad;
				input->mPaths.push_back(it.string());
			}
		} else if (is_image(it)) {
			if (input->mPaths.empty() || input->mType == InputType::kSave) {
				input->mType = InputType::kSave;
				input->mPaths.push_back(it.string());
			}
		}
	}
	return input;
}

void App::gifThread(ci::gl::ContextRef context) {
	ci::ThreadSetup					threadSetup;
	context->makeCurrent();

	while (!mQuit) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		try {
			auto					input = mThreadInput.pop();
			if (input) {
				if (input->mType == InputType::kLoad) {
					gifThreadLoad(input->mPaths, input->mReplaceNavigation);
				} else if (input->mType == InputType::kSave) {
					gifThreadSave(*input, input->mReplaceNavigation);
				}
			}
		} catch (std::exception const &ex) {
			std::cout << "Error=" << ex.what() << std::endl;
		}
	}
}

void App::gifThreadLoad(const StringVec &input, const bool replace_navigation) {
	auto				output = mThreadOutput.make();
	if (!output) return;

	// When we have a collection of paths as input, load the first
	// and let the user navigate through the others.
	output->mPaths = input;
	output->mReplaceNavigation = replace_navigation;
	if (!input.empty()) {
		auto			fn = input.front();
		mStatusTransport.push_back(Status(Status::Duration::kStart, ++mThreadStatusId, "Loading " + get_filename(fn)));
		gif::Reader(fn).read(output->mGifList);
		mStatusTransport.push_back(Status(Status::Duration::kEnd, mThreadStatusId, std::string()));
	}
	mThreadOutput.push(output);
}

void App::gifThreadSave(const Input &input, const bool replace_navigation) {
	mStatusTransport.push_back(Status(Status::Duration::kStart, ++mThreadStatusId, "Saving " + get_filename(input.mSavePath)));

	std::string				error;
	try {
		PathWriter			file(input.mSavePath);
		file.setTableMode(gif::TableMode::kGlobalTableFromFirst);
		gif::Bitmap			bm;
		for (const auto& it : input.mPaths) {
			if (!is_image(it)) continue;
			file.writeFrame(it);

			if (mQuit) break;
		}
	} catch (std::exception const &ex) {
		error = ex.what();
	}

	if (!error.empty()) {
		mStatusTransport.push_back(Status::makeError(error));
	}
	mStatusTransport.push_back(Status(Status::Duration::kEnd, mThreadStatusId, std::string()));
}

namespace {

bool						is_gif(const ci::fs::path &p) {
	std::string				ext = kt::to_lower(p.extension().string());
	return ext == ".gif";
}

bool						is_image(const ci::fs::path &p) {
	std::string				ext = kt::to_lower(p.extension().string());
	return ext == ".jpg" || ext == ".jpeg" || ext == ".png";
}

std::string					get_filename(std::string p) {
	return ci::fs::path(p).filename().string();
}

}

} // namespace cs

CINDER_APP(cs::App, ci::app::RendererGl, cs::App::prepareSettings)