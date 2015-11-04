#include "file_navigation_view.h"

#include <cinder/gl/draw.h>
#include <cinder/gl/gl.h>
#include "msg/app_msg.h"

namespace cs {

namespace {
kt::view::VisualRef		previous_visual(const kt::Cns&);
kt::view::VisualRef		next_visual(const kt::Cns&);
}

/**
 * @class cs::FileNavigationView
 */
FileNavigationView::FileNavigationView(kt::Cns &cns)
		: base(cns)
		, mPrevious(kt::view::Button::make(*this, cns, previous_visual(cns)))
		, mNext(kt::view::Button::make(*this, cns, next_visual(cns))) {
	glm::vec3	btn_size(32, 32, 0);
	mPrevious.setSize(btn_size);
	mNext.setSize(btn_size);
	mNext.setPosition(floorf(mPrevious.getPosition().x + mPrevious.getSize().x), mPrevious.getPosition().y);

	setSize(floorf(mNext.getPosition().x + mNext.getSize().x), floorf(mPrevious.getSize().y));

	mPrevious.setClickFn([this]() {
		if (mNavigation) {
			auto		previous = mNavigation->previous();
			if (!previous.empty()) {
				cs::SetMediaPathMsg(previous).send(mCns);
			}
		}
	});

	mNext.setClickFn([this]() {
		if (mNavigation) {
			auto		next = mNavigation->next();
			if (!next.empty()) {
				cs::SetMediaPathMsg(next).send(mCns);
			}
		}
	});
}

void FileNavigationView::setNavigation(FileNavigationRef r) {
	mNavigation = r;
}

namespace {

// XXX Temp classes
class PrevVis : public kt::view::Visual {
public:
	PrevVis() {  mC = mRestC; }

	void				setSize(const glm::vec3 &size) override {
		mSize = size;
	}

	void				setState(const std::string &s) override {
		if (s == kt::view::DOWN_STATE) mC = mDownC;
		else mC = mRestC;
	}

	void				onDraw(const kt::view::DrawParams&) override {
		ci::gl::color(mC);
		ci::gl::drawSolidRect(ci::Rectf(0.0f, 0.0f, mSize.x, mSize.y));	
	}

private:
	glm::vec3			mSize;
	ci::ColorA			mRestC = ci::ColorA(0.6f, 0.9f, 0.6f, 1.0f),
						mDownC = ci::ColorA(0.0f, 0.5f, 0.0f, 1.0f);
	ci::ColorA			mC;
};

class NextVis : public kt::view::Visual {
public:
	NextVis() { mC = mRestC; }

	void				setSize(const glm::vec3 &size) override {
		mSize = size;
	}

	void				setState(const std::string &s) override {
		if (s == kt::view::DOWN_STATE) mC = mDownC;
		else mC = mRestC;
	}

	void				onDraw(const kt::view::DrawParams&) override {
		ci::gl::color(mC);
		ci::gl::drawSolidRect(ci::Rectf(0.0f, 0.0f, mSize.x, mSize.y));	
	}

private:
	glm::vec3			mSize;
	ci::ColorA			mRestC = ci::ColorA(0.9f, 0.6f, 0.6f, 1.0f),
						mDownC = ci::ColorA(0.5f, 0.0f, 0.0f, 1.0f);
	ci::ColorA			mC;
};

kt::view::VisualRef		previous_visual(const kt::Cns&) {
	return std::make_shared<PrevVis>();
}

kt::view::VisualRef		next_visual(const kt::Cns&) {
	return std::make_shared<NextVis>();
}

}

} // namespace cs
