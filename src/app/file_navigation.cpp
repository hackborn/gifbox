#include "file_navigation.h"

#include <algorithm>
#include <cinder/Filesystem.h>
#include <kt/app/kt_string.h>

namespace cs {

namespace {
// True if the path is a GIF.
bool						is_gif(const ci::fs::path&);
}

/**
 * @class cs::DirectoryNavigation
 */
DirectoryNavigation::DirectoryNavigation(const std::string &starting_path) {
	// Cache the parent folder from the initial file
	cacheFolder(ci::fs::path(starting_path).parent_path());
	mCurrentIndex = indexFor(starting_path);
	if (mCurrentIndex >= mList.size()) mCurrentIndex = 0;
}

bool DirectoryNavigation::hasPrevious() const {
	if (mList.size() < 2) return false;
	return mCurrentIndex > 0;
}

bool DirectoryNavigation::hasNext() const {
	if (mList.size() < 2) return false;
	return mCurrentIndex < mList.size()-1;
}

std::string DirectoryNavigation::previous() {
	const size_t		v = mCurrentIndex;
	step(-1);
	if (mCurrentIndex == v || mCurrentIndex >= mList.size()) return std::string();
	return mList[mCurrentIndex].string();
}

std::string DirectoryNavigation::next() {
	const size_t		v = mCurrentIndex;
	step(1);
	if (mCurrentIndex == v || mCurrentIndex >= mList.size()) return std::string();
	return mList[mCurrentIndex].string();
}

void DirectoryNavigation::step(int32_t delta) {
	if (mList.size() < 2) {
		mCurrentIndex = 0;
		return;
	}
	int32_t		new_value = static_cast<int32_t>(mCurrentIndex) + delta;
	if (new_value < 0) new_value = 0;
	else if (new_value >= static_cast<int32_t>(mList.size())) new_value = mList.size()-1;
	mCurrentIndex = static_cast<size_t>(new_value);
}

void DirectoryNavigation::cacheFolder(const ci::fs::path &parent) {
	// Eventually C++ will have filesystem support.
	namespace csfs = ci::fs;
	try {
		mList.clear();
		mCurrentIndex = 0;
		mParent = parent;
		// This won't do anything on FAT volumes, which don't change the parent folder
		// modified timestamp when items are added or removed, but it works on NTFS.
		mParentModifiedTime = csfs::last_write_time(parent);

		csfs::directory_iterator	end;
		for (csfs::directory_iterator it(parent); it != end; ++it) {
			// Valid file?
			if (!csfs::is_regular_file(it->status())) continue;
			const csfs::path&		path = it->path();
			if (is_gif(path)) {
				mList.push_back(path);
			}
		}
		std::sort(mList.begin(), mList.end());
	} catch (std::exception const&) {
	}
}

size_t DirectoryNavigation::indexFor(const std::string &path) const {
	const ci::fs::path		p(path);
	for (size_t k=0; k<mList.size(); ++k) {
		if (mList[k] == p) return k;
	}
	return mList.size();
}

/**
 * @class cs::FileListNavigation
 */
FileListNavigation::FileListNavigation(const std::vector<std::string> &l)
		: mList(l) {
}

bool FileListNavigation::hasPrevious() const {
	return true;
}

bool FileListNavigation::hasNext() const {
	return true;
}

std::string FileListNavigation::previous() {
	return std::string();
}

std::string FileListNavigation::next() {
	return std::string();
}

namespace {

bool						is_gif(const ci::fs::path &p) {
	std::string				ext = kt::to_lower(p.extension().string());
	return ext == ".gif";
}

}

} // namespace cs
