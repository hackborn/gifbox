#ifndef KT_VIEW_VIEW_H_
#define KT_VIEW_VIEW_H_

#include <memory>
#include <vector>
#include <cinder/Color.h>
#include <cinder/Matrix44.h>
#include <kt/app/pointer.h>
#include <kt/app/update_defs.h>
#include <kt/dbg/dbg_defs.h>
#include <kt/view/view_defs.h>
// I separated the msg stuff specifically so I could swap it out,
// but it's so useful it's making its way back in.
#include <kt/service/msg_service.h>

namespace kt {
class Cns;
namespace view {
class View;
using ViewRef = std::shared_ptr<View>;
using ViewList = std::vector<ViewRef>;
extern const std::uint32_t		HIT_POINTER_EVENTS;
extern const std::uint32_t		HIT_DRAG_EVENTS;

/**
 * @class kt::view::View
 * @brief Abstract drawable.
 */
class View {
public:
	using Iter = ViewList::iterator;
	using CIter = ViewList::const_iterator;

	View() = delete;
	View(const View&) = delete;
	View(kt::Cns&);
	virtual ~View();

	void						update(const kt::UpdateParams&);
	void						draw(const kt::view::DrawParams&);

	kt::Cns&					getCns() { return mCns; }

	void						setParent(View*);
	void						addChild(View*);
	void						addChild(const ViewRef&);
	// Add a type that must be a view subclass. Throw if nullptr. This is
	// a convenience for classes that will never remove the child, and a
	// little bit of a hold over from a previous design that didn't memory
	// manage the children.
	template <typename T>
	T&							addChildOrThrow(T*);
	void						removeChild(const ViewRef&);
	void						removeChildren(const ViewList&);
	void						clearChildren();

	ViewRef						findChild(const std::function<bool(const View&)>&) const;

	Iter						begin() { return mChildren.begin(); }
	Iter						end() { return mChildren.end(); }
	CIter						begin() const { return mChildren.begin(); }
	CIter						end() const { return mChildren.end(); }

	// Generic message handling
	virtual void				onMsg(const kt::Msg&) { }
	// Send a message up the hierarchy
	void						parentMsg(const kt::Msg&);
	// Abstract interface for changing state.
	virtual void				setVisualState(const std::string&) { }

	// FLAGS
	// When true this view will not draw.
	void						setHidden(const bool = false);
	bool						isHidden() const;

	View&						setPosition(const float x, const float y, const float z = 0.0f);
	View&						setPosition(const glm::vec3&);
	glm::vec3					getPosition() const									{ return mPosition; }

	View&						setSize(const float x, const float y, const float z = 0.0f);
	View&						setSize(const glm::vec3&);
	View&						setSize(const glm::vec2 &xy) { return setSize(glm::vec3(xy.x, xy.y, 0.0f)); }
	glm::vec3					getSize() const										{ return mSize; }

	View&						setScale(const float x, const float y, const float z = 1.0f);
	View&						setScale(const glm::vec3&);
	View&						setScale(const glm::vec2 &xy) { return setScale(glm::vec3(xy.x, xy.y, 1.0f)); }
	glm::vec3					getScale() const									{ return mScale; }

	View&						setRotation(const float x, const float y, const float z) { return setRotation(glm::vec3(x, y, z)); }
	View&						setRotation(const glm::vec3&);
	glm::vec3					getRotation() const									{ return mRotation; }

	View&						setCenter(const float x, const float y, const float z = 0.0f) { return setCenter(glm::vec3(x, y, z)); }
	View&						setCenter(const glm::vec3&);
	glm::vec3					getCenter() const									{ return mCenter; }

	// Setting the non-A color will preserve the current alpha value.
	View&						setColor(const ci::Color&);
	View&						setColor(const ci::ColorA&);

	// TOUCH HIT
	// Set the flags that will trigger a hit.
	void						setPointerEvents(const bool = false);
	void						setDragEvents(const bool = false);
	virtual ViewRef				getHit(	const glm::vec3 &global_point, const ViewRef &my_container,
										const std::uint32_t hit_flags) const;
	bool						contains(const glm::vec3 &global_point) const;

	// Pointer handling. Default implementation passes to the currently-assigned
	// pointer handler, if any.
	static const std::uint32_t	INSIDE_DOWN_VIEW = 1<<0;
	virtual void				pointerDown(const Pointer&);
	virtual void				pointerMoved(const Pointer&, const std::uint32_t flags = 0);
	virtual void				pointerUp(const Pointer&);

	// Translations
	// XXX These should probably be placed in a separate util that caches the result
	// of building the matrix.
	glm::vec3					globalFromLocal(const glm::vec3&) const;
	glm::vec3					localFromGlobal(const glm::vec3&) const;

protected:
	virtual void				onUpdate(const kt::UpdateParams&) { }
	virtual void				onDraw(const kt::view::DrawParams&) { }

	void						setFlag(const std::uint32_t mask, const bool);
	bool						getFlag(const std::uint32_t mask) const;

	virtual void				onSetPosition(const glm::vec3&);
	virtual void				onSetSize(const glm::vec3&);
	virtual void				onSetScale(const glm::vec3&);
	virtual void				onSetRotation(const glm::vec3&);
	virtual void				onSetCenter(const glm::vec3&);
	virtual void				onSetColor(const ci::ColorA&);

	virtual void				onPositionChanged(const glm::vec3&) { }
	virtual void				onSizeChanged(const glm::vec3&) { }
	virtual void				onScaleChanged(const glm::vec3&) { }
	virtual void				onRotationChanged(const glm::vec3&) { }
	virtual void				onCenterChanged(const glm::vec3&) { }
	virtual void				onColorChanged(const ci::ColorA&) { }

	const glm::mat4&			getLocalTransform() const;

	// Query up the parent chain until I get I find a non-nullptr cfg;
	RootCfgRef					getParentRootCfg() const;

	friend class Root;
	kt::Cns&					mCns;
	View*						mParent = nullptr;
	ViewList					mChildren;
	RootCfgRef					mRootCfg;

	glm::vec3					mPosition,
								mSize = glm::vec3(0.0f, 0.0f, 0.0f),
								mScale = glm::vec3(1.0, 1.0f, 1.0f),
								mRotation = glm::vec3(0.0f, 0.0f, 0.0f),
								mCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	ci::ColorA					mColor;

private:
	void						setHitFlag(const std::uint32_t mask, const bool);
	// For now, this is built each time I need it.
	glm::mat4					getGlobalTransform() const;

	std::uint32_t				mFlags = 0;
	std::uint32_t				mHitFlags = 0;
	// Cached
	mutable bool				mTransformDirty = true;
	mutable glm::mat4			mTransform;
};

/**
 * VIEW IMPLEMENTATION
 */
template <typename T>
T& View::addChildOrThrow(T *view) {
	if (!view) throw std::runtime_error("View::addChildOrThrow() on nullptr");
	addChild(ViewRef(view));
	return *view;
}

} // namespace view
} // namespace kt

#endif