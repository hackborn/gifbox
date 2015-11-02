#ifndef KT_VIEW_VIEWCURTAIN_H_
#define KT_VIEW_VIEWCURTAIN_H_

#include "view.h"

namespace kt {
namespace view {

/**
 * @class kt::view::Curtain
 * @brief A curtain is used to recycle views from a parent, by treating hidden views
 * as unused. So "clearing" a curtain means hiding all views. The parent view owns
 * all views in the curtain (so clearing out its children will cause a crash), but
 * the curtain maintains a list of all views it added, so a view can have multiple
 * curtains.
 */
template <typename T>
class Curtain {
public:
	using Container = std::vector<T*>;
	using Iter = typename Container::iterator;
	using CIter = typename Container::const_iterator;

	Curtain() = delete;
	Curtain(const Curtain&) = delete;
	Curtain(View &parent);

	// Hide all of parent's views.
	void						clear();
	// Create or unhide a new view.
	T*							start();
	// Set a function used to initialize any new views
	void						setInitializationFn(const std::function<void(T&)> & fn) { mInitializationFn = fn; }

	// Iterate over all views, even hidden ones.
	Iter						begin() { return mContainer.begin(); }
	Iter						end() { return mContainer.end(); }
	CIter						begin() const { return mContainer.begin(); }
	CIter						end() const { return mContainer.end(); }

private:
	View&						mParent;
	Container					mContainer;
	std::function<void(T&)>		mInitializationFn;
};

template <typename T>
Curtain<T>::Curtain(View &parent)
		: mParent(parent) {
	// All classes required to be views
	static_assert(std::is_base_of<kt::view::View, T>::value, "kt::view::Curtain template class must be derived from kt::view::View");
}

template <typename T>
void Curtain<T>::clear() {
	for (auto& v : mContainer) {
		v->setHidden(true);
	}
}

template <typename T>
T* Curtain<T>::start() {
	for (auto& v : mContainer) {
		if (!v->isHidden()) continue;
		T*			ans = dynamic_cast<T*>(v);
		if (ans) {
			ans->setHidden(false);
			return ans;
		}
	}
	// XXX Be nice to allow custom constructors
	try {
		T*				ans = new T(mParent.getCns());
		if (ans) {
			if (mInitializationFn) mInitializationFn(*ans);
			mContainer.push_back(ans);
			mParent.addChild(ans);
		}
		return ans;
	} catch (std::exception const&) {
	}
	return nullptr;
}

} // namespace view
} // namespace kt

#endif