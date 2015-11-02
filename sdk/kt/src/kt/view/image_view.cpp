#include "image_view.h"

#include <cinder/ImageIo.h>
#include <cinder/gl/draw.h>
#include <cinder/gl/scoped.h>
#include <kt/app/cns.h>

namespace kt {
namespace view {

/**
 * @class kt::view::Image
 */
Image::Image(kt::Cns &cns)
		: base(cns) {
	setColor(ci::ColorA(1.0f, 1.0f, 1.0f, 1.0f));
}

bool Image::empty() const {
	return !mTexture;
}

void Image::setFilename(const std::string &fn) {
	assignTexture([&fn]()->ci::gl::TextureRef { return ci::gl::Texture::create(ci::loadImage(fn));});
}

void Image::setSurface(const ci::Surface8u &s) {
	assignTexture([&s]()->ci::gl::TextureRef { return ci::gl::Texture::create(s);});
}

namespace {
// XXX Should be in a math class, and should be constexpr which -- WTF??
// -- it doesn't look like is supported even in VS2015.
const float PI = 3.1415926535897932f;
const float RAD2DEG = 180.0f / PI;
const float DEG2RAD = PI / 180.0f;
}

void Image::onDraw(const kt::view::DrawParams &p) {
	base::onDraw(p);
	if (mTexture) {
		ci::gl::color(mColor);

		ci::gl::ScopedModelMatrix	smm;

		// XXX Here is where we're accounting for the inverted texture.
		// With new version will come better tools for dealing with this.
		glm::vec3					rotation(180.0f, 0.0f, 0.0f),
									position(0.0f, static_cast<float>(mTexture->getHeight()), 0.0f);
		ci::gl::translate(position);
		ci::gl::rotate(rotation.x * DEG2RAD, glm::vec3(1.0f, 0.0f, 0.0f));
		ci::gl::rotate(rotation.y * DEG2RAD, glm::vec3(0.0f, 1.0f, 0.0f));
		ci::gl::rotate(rotation.z * DEG2RAD, glm::vec3(0.0f, 0.0f, 1.0f));

		// Prevent writing to the depth buffer, which will block out
		// pixels that are supposed to be transparent.
		ci::gl::ScopedDepthWrite	sdw(false);

		ci::gl::draw(mTexture);
	}
}

void Image::assignTexture(const std::function<ci::gl::TextureRef(void)> &fn) {
	glm::vec3		new_size(glm::vec3(0.0f, 0.0f, 0.0f));
	try {
		mTexture.reset();
		if (fn) mTexture = fn();
		if (mTexture) {
			new_size.x = static_cast<float>(mTexture->getWidth());
			new_size.y = static_cast<float>(mTexture->getHeight());
		}
	} catch (std::exception const&) {
	}
	setSize(new_size);
}

} // namespace view
} // namespace kt

