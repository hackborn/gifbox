#include "label_view.h"

#include <cinder/gl/scoped.h>
#include <kt/app/cns.h>

namespace kt {
namespace view {

/**
 * @class kt::view::Label
 */
Label::Label(kt::Cns &cns)
		: base(cns)
		, mFontService(cns.getService<kt::FontService>(kt::FontService::NAME()))
		, mAscent(0.0f)
		, mDescent(0.0f) {
	setFont("");
	setColor(ci::ColorA(1.0f, 1.0f, 1.0f, 1.0f));
//setBackgroundColor(ci::ColorA(0.0f, 0.0f, 1.0f, 0.2f));
}

void Label::setFont(const std::string &key) {
	mFont = mFontService.getFont(key);
	if (mFont) {
		mAscent = mFont->getAscent();
		mDescent = mFont->getDescent();
	}
	setSize(mSize.x, ceilf(mAscent+mDescent), 0.0f);
}

void Label::setText(const std::string &text) {
	mText = text;
	measureSize();
}

namespace {
// XXX Should be in a math class, and should be constexpr which -- WTF??
// -- it doesn't look like is supported even in VS2015.
const float PI = 3.1415926535897932f;
const float RAD2DEG = 180.0f / PI;
const float DEG2RAD = PI / 180.0f;
}

void Label::onDraw(const kt::view::DrawParams &p) {
	base::onDraw(p);
	if (mFont && !mText.empty()) {
		ci::gl::color(mColor);

		// Prevent writing to the depth buffer, which will block out
		// pixels that are supposed to be transparent.
		ci::gl::ScopedDepthWrite	sdw(false);
		ci::gl::ScopedModelMatrix	smm;
		// XXX Here is where we're accounting for the inverted texture.
		// With new version will come better tools for dealing with this.
		const bool					invert_y = isInvertedY();
		if (invert_y) {
			glm::vec3				rotation(180.0f, 0.0f, 0.0f),
									position(0.0f, mSize.y, 0.0f);
			ci::gl::translate(position);
			ci::gl::rotate(rotation.x * DEG2RAD, glm::vec3(1.0f, 0.0f, 0.0f));
			ci::gl::rotate(rotation.y * DEG2RAD, glm::vec3(0.0f, 1.0f, 0.0f));
			ci::gl::rotate(rotation.z * DEG2RAD, glm::vec3(0.0f, 0.0f, 1.0f));
		}

		mFont->drawString(mText, glm::vec2(0.0f, mAscent));
	}
}

void Label::measureSize() {
	float					w(0.0f), h(0.0f);
	if (mFont && !mText.empty()) {
		const glm::vec2		size(mFont->measureString(mText));
		w = size.x;
	}
	mSize.x = floorf(w);
//	mSize.y = floorf(h);
}

bool Label::isInvertedY() {
	if (mRootCfg) return mRootCfg->mInvertY;
	mRootCfg = getParentRootCfg();
	if (mRootCfg) return mRootCfg->mInvertY;
	return false;
}

} // namespace view
} // namespace kt

