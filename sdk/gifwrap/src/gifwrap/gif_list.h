#ifndef GIFWRAP_GIFLIST_H_
#define GIFWRAP_GIFLIST_H_

#include <functional>
#include "gif_bitmap.h"

namespace gif {

/**
 * @class gif::ListConstructor
 * @brief A stub class passed to the framework for constructing lists.
 */
class ListConstructor {
public:
	ListConstructor() { }
	virtual ~ListConstructor() { }

	virtual void			addFrame(const gif::Bitmap&, const double delay) = 0;
	// Called when the if reader is done reading frames, so
	// any resources can be cleaned up.
	virtual void			readerFinished() { }
};

/**
 * @class gif::GifList
 * @brief Templated class that defines a collection of image frames.
 * @description Clients will subclass and specialize to provide both a
 * local bitmap storage format as T as well as an allocator that translates
 * gif bitmaps into the local storage class.
 */
template <typename T>
class List : public gif::ListConstructor {
public:
	class Frame {
	public:
		Frame() { }

		T							mBitmap;
		double						mDelay = 0.0;
	};

public:
	List(const std::function<T(const gif::Bitmap&)>& alloc = nullptr) : mAlloc(alloc) { }

	bool							empty() const { return mFrames.empty(); }
	size_t							size() const { return mFrames.size(); }

	void							addFrame(const gif::Bitmap&, const double delay) override;
	const Frame*					getFrame(const size_t index) const;

protected:
	std::function<T(const gif::Bitmap&)>
									mAlloc;
	std::vector<Frame>				mFrames;
};

/**
 * gif::List IMPLEMENTATION
 */
template <typename T>
void List<T>::addFrame(const gif::Bitmap &bm, const double delay) {
	mFrames.push_back(Frame());
	Frame&			f(mFrames.back());
	if (mAlloc) f.mBitmap = mAlloc(bm);
	f.mDelay = delay;
}

template <typename T>
const typename List<T>::Frame* List<T>::getFrame(const size_t index) const {
	if (index >= mFrames.size()) return nullptr;
	return &mFrames[index];
}

} // namespace gif

#endif
