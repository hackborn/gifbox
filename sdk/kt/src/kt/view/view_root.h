#ifndef KT_VIEW_VIEWROOT_H_
#define KT_VIEW_VIEWROOT_H_

#include <cinder/Area.h>
#include <cinder/Camera.h>
#include "view.h"

namespace kt {
namespace view {
class Root;
using RootRef = std::shared_ptr<Root>;
using RootList = std::vector<RootRef>;

/**
 * @class kt::view::Root
 * @brief Abstract root container for views. Subclasses provide camera info.
 */
class Root {
public:
	virtual ~Root();

	void						update(const kt::UpdateParams&);
	void						draw(const kt::view::DrawParams&);

	void						addChild(View*);
	void						addChild(const ViewRef&);
	// Add a type that must be a view subclass. Throw if nullptr. This is
	// a convenience for classes that will never remove the child, and a
	// little bit of a hold over from a previous design that didn't memory
	// manage the children.
	template <typename T>
	T&							addChildOrThrow(T*);

	void						setViewport(const ci::Area &a) { mViewportArea = a; }

	ViewRef						getHit(const glm::vec3 &global_point, const std::uint32_t hit_flags) const;

protected:
	Root(kt::Cns&);

	virtual void				onUpdate(const kt::UpdateParams&) { }
	virtual void				onDraw(const kt::view::DrawParams&) { }

	kt::Cns&					mCns;
	std::vector<ViewRef>		mChildren;
	ci::Area					mViewportArea;
	kt::view::RootCfgRef		mRootCfg;
};

/**
 * @class kt::view::PerspRoot
 * @brief Perspective camera root.
 */
class PerspRoot : public Root {
public:
	// For API simplicity, set the up at construction and never change it. Not sure
	// why you'd ever want to.
	PerspRoot(kt::Cns&, const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f));

	// Camera
	void						setCameraPerspective(float verticalFovDegrees, float aspectRatio, float nearPlane, float farPlane);
	void						cameraLookAt(const glm::vec3 &aEyePoint, const glm::vec3 &target);
	ci::CameraPersp&			getCamera() { return mCamera; }
	const ci::CameraPersp&		getCamera() const { return mCamera; }

private:
	void						onDraw(const kt::view::DrawParams&) override;

	using base = Root;
	ci::CameraPersp				mCamera;
};

/**
 * @class kt::view::OrthoRoot
 * @brief Orthogonal camera root.
 */
class OrthoRoot : public Root {
public:
	OrthoRoot(kt::Cns&);

	// Camera
	void						setCameraOrtho(float left, float right, float bottom, float top, float nearPlane, float farPlane);
	const ci::CameraOrtho&		getCamera() const { return mCamera; }

private:
	void						onDraw(const kt::view::DrawParams&) override;

	using base = Root;
	ci::CameraOrtho				mCamera;
};

/**
 * VIEW-ROOT IMPLEMENTATION
 */
template <typename T>
T& Root::addChildOrThrow(T *view) {
	if (!view) throw std::runtime_error("kt::view::Root::addChildOrThrow() on nullptr");
	addChild(ViewRef(view));
	return *view;
}

} // namespace view
} // namespace kt

#endif