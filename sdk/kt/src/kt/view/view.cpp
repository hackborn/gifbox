#include "view.h"

#include <cinder/gl/gl.h>
#include <cinder/gl/scoped.h>

namespace kt {
namespace view {

const std::uint32_t			HIT_POINTER_EVENTS(1<<0);
const std::uint32_t			HIT_DRAG_EVENTS(1<<1);

namespace {
// XXX Should be in a math class, and should be constexpr which -- WTF??
// -- it doesn't look like is supported even in VS2015.
const float PI = 3.1415926535897932f;
const float RAD2DEG = 180.0f / PI;
const float DEG2RAD = PI / 180.0f;

// Flags
const std::uint32_t			HIDDEN_F(1<<0);
const std::uint32_t			CLIPPING_F(1<<1);
}

/**
 * @class kt::view::View
 */
View::View(kt::Cns &cns)
		: mCns(cns)
		, mColor(1.0f, 1.0f, 1.0f, 1.0f) {
}

View::~View() {
}

void View::update(const kt::UpdateParams &p) {
	onUpdate(p);
	for (auto& it : mChildren) {
		if (it) it->update(p);
	}
}

void View::draw(const kt::view::DrawParams &p) {
	if (isHidden()) return;

	ci::gl::ScopedModelMatrix	smm;
	auto ctx = ci::gl::context();
	ctx->getModelMatrixStack().back() *= getLocalTransform();

	onDraw(p);
	for (auto& it : mChildren) {
		if (it) it->draw(p);
	}
}

void View::setParent(View *v) {
	mParent = v;
}

void View::addChild(View *v) {
	addChild(ViewRef(v));
}

void View::addChild(const ViewRef &vr) {
	if (!vr) return;
	try {
		mChildren.push_back(vr);
		vr->mParent = this;
	} catch (std::exception const&) {
	}
}

void View::removeChild(const ViewRef &v) {
	if (!v) return;
	try {
		mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), v), mChildren.end());
	} catch (std::exception const&) {
	}
}

void View::removeChildren(const ViewList &list) {
	for (const auto& it : list) {
		removeChild(it);
	}
}

void View::clearChildren() {
	mChildren.clear();
}

ViewRef View::findChild(const std::function<bool(const View&)> &fn) const {
	if (!fn) return ViewRef();
	for (auto& it : mChildren) {
		if (it && fn(*(it.get()))) return it;
	}
	return ViewRef();
}

void View::setHidden(const bool v) {
	const std::uint32_t		old_flags(mFlags);
	setFlag(HIDDEN_F, v);
//	if (mFlags != old_flags && mHooks) mHooks->onHiddenChanged(v);
}

bool View::isHidden() const {
	return getFlag(HIDDEN_F);
}

View& View::setPosition(const float x, const float y, const float z) {
	return setPosition(glm::vec3(x, y, z));
}

View& View::setPosition(const glm::vec3 &v) {
	onSetPosition(v);
	return *this;
}

View& View::setSize(const float x, const float y, const float z) {
	return setSize(glm::vec3(x, y, z));
}

View& View::setSize(const glm::vec3 &v) {
	onSetSize(v);
	return *this;
}

View& View::setScale(const float x, const float y, const float z) {
	return setScale(glm::vec3(x, y, z));
}

View& View::setScale(const glm::vec3 &v) {
	onSetScale(v);
	return *this;
}

View& View::setRotation(const glm::vec3 &v) {
	onSetRotation(v);
	return *this;
}


View& View::setCenter(const glm::vec3 &v) {
	onSetCenter(v);
	return *this;
}

View& View::setColor(const ci::Color &v) {
	onSetColor(ci::ColorA(v.r, v.g, v.b, mColor.a));
	return *this;
}

View& View::setColor(const ci::ColorA &v) {
	onSetColor(v);
	return *this;
}

void View::setPointerEvents(const bool v) {
	setHitFlag(HIT_POINTER_EVENTS, v);
}

void View::setDragEvents(const bool v) {
	setHitFlag(HIT_DRAG_EVENTS, v);
}

ViewRef View::getHit(const glm::vec3 &point, const ViewRef &my_container, const std::uint32_t hit_flags) const {
	if (isHidden()) return ViewRef();
	if (!contains(point)) return ViewRef();
	// Reverse iterate because conceptually, things on the bottom of the list are at the front of the display order
	for (auto it = mChildren.rbegin(), end=mChildren.rend(); it!=end; ++it) {
		const ViewRef&		ref(*it);
		if (!ref) continue;
		ViewRef				ans(ref->getHit(point, ref, hit_flags));
		if (ans) return ans;
	}
	if ((mHitFlags&hit_flags) == 0) return ViewRef();
	return my_container;
}

bool View::contains(const glm::vec3 &point) const {
	// Pulled from downstream ds_cinder/sprite.cpp
	if (mSize.x <= 0.0f || mSize.y <= 0.0f) return false;
	if (mScale.x <= 0.0f || mScale.y <= 0.0f) return false;

	const float			pad(0.0f);
	glm::mat4			global_m(getGlobalTransform());

	glm::vec4 pR = glm::vec4(point.x, point.y, point.z, 1.0f);
	glm::vec4 cA = global_m * glm::vec4(-pad,			-pad,			0.0f, 1.0f);
	glm::vec4 cB = global_m * glm::vec4(mSize.x + pad,	-pad,			0.0f, 1.0f);
	glm::vec4 cC = global_m * glm::vec4(mSize.x + pad,	mSize.y + pad,	0.0f, 1.0f);
	
	glm::vec4 v1 = cA - cB;
	glm::vec4 v2 = cC - cB;
	glm::vec4 v = pR - cB;

	float dot1 = glm::dot(v, v1);
	float dot2 = glm::dot(v, v2);
	float dot3 = glm::dot(v1, v1);
	float dot4 = glm::dot(v2, v2);

	return (
		dot1 >= 0 &&
		dot2 >= 0 &&
		dot1 <= dot3 &&
		dot2 <= dot4
	);
}

void View::pointerDown(const Pointer &p) {
//	if (mPointerHandler) mPointerHandler->pointerDown(p);
}

void View::pointerMoved(const Pointer &p, const std::uint32_t flags) {
//	if (mPointerHandler) mPointerHandler->pointerMoved(p, flags);
}

void View::pointerUp(const Pointer &p) {
//	if (mPointerHandler) mPointerHandler->pointerUp(p);
}

glm::vec3 View::globalFromLocal(const glm::vec3 &local_pt) const {
	const glm::mat4			m(getGlobalTransform());
	const glm::vec4			pt(m * glm::vec4(local_pt.x, local_pt.y, local_pt.z, 1.0f));
	return glm::vec3(pt.x, pt.y, pt.z);
}

glm::vec3 View::localFromGlobal(const glm::vec3 &global_pt) const {
	const glm::mat4			m(glm::inverse(getGlobalTransform()));
	const glm::vec4			pt(m * glm::vec4(global_pt.x, global_pt.y, global_pt.z, 1.0f));
	return glm::vec3(pt.x, pt.y, pt.z);
}

void View::setFlag(const std::uint32_t mask, const bool v) {
	if (v) mFlags |= mask;
	else mFlags &= ~mask;
}

bool View::getFlag(const std::uint32_t mask) const {
	return (mFlags&mask) != 0;
}

void View::onSetPosition(const glm::vec3 &v) {
	if (mPosition == v) return;
	mPosition = v;
	mTransformDirty = true;
//	if (mHooks) mHooks->onPositionChanged(mPosition);
	onPositionChanged(v);
}

void View::onSetSize(const glm::vec3 &v) {
	if (mSize == v) return;
	mSize = v;
	mTransformDirty = true;
//	if (mHooks) mHooks->onSizeChanged(mSize);
	onSizeChanged(v);
}

void View::onSetScale(const glm::vec3 &v) {
	if (mScale == v) return;
	mScale = v;
//	if (mHooks) mHooks->onSizeChanged(mSize);
	onScaleChanged(v);
}

void View::onSetRotation(const glm::vec3 &v) {
	if (mRotation == v) return;
	mRotation = v;
	mTransformDirty = true;
//	if (mHooks) mHooks->onRotationChanged(mRotation);
	onRotationChanged(v);
}

void View::onSetCenter(const glm::vec3 &v) {
	if (mCenter == v) return;
	mCenter = v;
	mTransformDirty = true;
//	if (mHooks) mHooks->onRotationChanged(mRotation);
	onCenterChanged(v);
}

void View::onSetColor(const ci::ColorA &v) {
	if (mColor == v) return;
	mColor = v;
	onColorChanged(v);
}
const glm::mat4& View::getLocalTransform() const {
	if (mTransformDirty) {
		mTransformDirty = false;
		mTransform = glm::mat4();
		mTransform = glm::translate(mTransform, mPosition);
		mTransform = glm::rotate(mTransform, mRotation.x * DEG2RAD, glm::vec3(1.0f, 0.0f, 0.0f));
		mTransform = glm::rotate(mTransform, mRotation.y * DEG2RAD, glm::vec3(0.0f, 1.0f, 0.0f));
		mTransform = glm::rotate(mTransform, mRotation.z * DEG2RAD, glm::vec3(0.0f, 0.0f, 1.0f));
		mTransform = glm::scale(mTransform, mScale);
		mTransform = glm::translate(mTransform, glm::vec3(-mCenter.x*mSize.x, -mCenter.y*mSize.y, -mCenter.z*mSize.z));
	}
	return mTransform;
}

RootCfgRef View::getParentRootCfg() const {
	View*			p = mParent;
	while (p != nullptr) {
		if (p->mRootCfg) return p->mRootCfg;
		p = p->mParent;
	}
	return nullptr;
}

void View::setHitFlag(const std::uint32_t mask, const bool v) {
	if (v) mHitFlags |= mask;
	else mHitFlags &= ~mask;
}

glm::mat4 View::getGlobalTransform() const {
	glm::mat4		g(getLocalTransform());
	for (View *v=mParent; v; v=v->mParent) {
		g = v->getLocalTransform() * g;
	}
	return g;
}

} // namespace view
} // namespace kt