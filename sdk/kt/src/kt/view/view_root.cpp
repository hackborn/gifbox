#include "view_root.h"

#include <cinder/gl/gl.h>

namespace kt {
namespace view {

/**
 * @class kt::view::Root
 */
Root::Root(kt::Cns &cns)
		: mCns(cns) {
}

Root::~Root() {
}

void Root::update(const kt::UpdateParams &p) {
	onUpdate(p);
	for (auto& it : mChildren) {
		if (it) it->update(p);
	}
}

void Root::draw(const kt::view::DrawParams &p) {
	onDraw(p);
	for (auto& it : mChildren) {
		if (it) it->draw(p);
	}
}

void Root::addChild(View *v) {
	addChild(ViewRef(v));
}

void Root::addChild(const ViewRef &vr) {
	if (!vr) return;
	try {
		vr->mRootCfg = mRootCfg;
		mChildren.push_back(vr);
	} catch (std::exception const&) {
	}
}

ViewRef Root::getHit(const glm::vec3 &global_point, const std::uint32_t hit_flags) const {
	for (const auto& it : mChildren) {
		if (!it) continue;

		ViewRef ans = it->getHit(global_point, it, hit_flags);
		if (ans) return ans;
	}
	return nullptr;
}

/**
 * @class kt::view::PerspRoot
 */
PerspRoot::PerspRoot(kt::Cns &cns, const glm::vec3 &up)
		: base(cns) {
	mCamera.setWorldUp(up);
	mRootCfg = std::make_shared<RootCfg>(true);
}

void PerspRoot::setCameraPerspective(float verticalFovDegrees, float aspectRatio, float nearPlane, float farPlane) {
	mCamera.setPerspective(verticalFovDegrees, aspectRatio, nearPlane, farPlane);
}

void PerspRoot::cameraLookAt(const glm::vec3 &aEyePoint, const glm::vec3 &target) {
	mCamera.lookAt(aEyePoint, target);
}

void PerspRoot::onDraw(const kt::view::DrawParams&) {
//	ci::gl::setViewport(mViewportArea);
	ci::gl::setMatrices(mCamera);

	// enable the depth buffer (after all, we are doing 3D)
	ci::gl::enableDepthRead();
	ci::gl::enableDepthWrite();
}

/**
 * @class kt::view::OrthoRoot
 */
OrthoRoot::OrthoRoot(kt::Cns &cns)
		: base(cns) {
	mRootCfg = std::make_shared<RootCfg>(false);
}

void OrthoRoot::setCameraOrtho(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
	mCamera.setOrtho(left, right, bottom, top, nearPlane, farPlane);
}

void OrthoRoot::onDraw(const kt::view::DrawParams&) {
//	ci::gl::setViewport(mViewportArea);
	ci::gl::setMatrices(mCamera);

	ci::gl::disableDepthRead();
	ci::gl::disableDepthWrite();
}

} // namespace view
} // namespace kt