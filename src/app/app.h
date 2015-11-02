#ifndef APP_APP_H_
#define APP_APP_H_

#include "cinder/params/Params.h"

#include <cinder/app/App.h>
#include <cinder/gl/Context.h>
#include <cinder/Camera.h>
#include <cinder/Thread.h>
#include <kt/app/app.h>
#include "app/status.h"
#include "thread/safe_value.h"
#include "thread/safe_vector.h"
#include "view/gif_view.h"

namespace cs {

/**
 * @class cs::App
 */
class App : public kt::App {
public:
	App();
	~App();

	static void					prepareSettings(Settings*);
	void						setup() override;

	void						keyDown(ci::app::KeyEvent) override;
	void						fileDrop(ci::app::FileDropEvent) override;

	void						update() override;
	void						draw() override;

private:
	using base = kt::App;
	using StringVec = std::vector<std::string>;

	enum class InputType		{ kLoad, kSave };
	struct Input {
		InputType				mType = InputType::kLoad;
		StringVec				mPaths;
		std::string				mSavePath;
	};

	std::shared_ptr<Input>		makeInput(const ci::app::FileDropEvent&) const;
	// Separate thread where all the file loading and saving occurs.
	void						gifThread(ci::gl::ContextRef);
	void						gifThreadLoad(const std::vector<std::string>&);
	void						gifThreadSave(const Input&);

	// Drawing
	kt::view::OrthoRoot&		mRoot;
	kt::view::View&				mHudView;
	cs::GifView&				mGifView;

	// Params
	ci::params::InterfaceGlRef	mParams;
	int32_t						mFrame = 0;
	float						mPlaybackSpeed = 1.0f;

	// Gif loading
	std::thread					mThread;
	std::atomic_bool			mQuit;
	SafeValue<Input>			mThreadInput;
	SafeValue<TextureGifList>	mThreadOutput;
	uint32_t					mThreadStatusId = 0;

	// Status
	using StatusVector = SafeVector<Status>;
	StatusVector				mStatusTransport;
	StatusVector::Container		mStatus;
};

} // namespace cs

#endif