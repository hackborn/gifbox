#include "file_navigation.h"

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
DirectoryNavigation::DirectoryNavigation(const std::string &starting_path)
		: mCurrentPath(starting_path) {
}

bool DirectoryNavigation::hasPrevious() const {
	return true;
}

bool DirectoryNavigation::hasNext() const {
	return true;
}

namespace {

std::string		move_file(const std::string &current, const std::function<void(ci::fs::directory_iterator&)> &step_fn) {
	if (!step_fn) return std::string();

	try {
		ci::fs::directory_iterator	end;
		ci::fs::path				fn(ci::fs::path(current).filename());
		ci::fs::directory_iterator	start(ci::fs::path(current).parent_path());
		auto it = std::find_if(start, end, [&fn](const ci::fs::directory_entry &e)->bool {
			return e.path().filename() == fn;
		});
		// Doesn't exist, that's a problem
		if (it == end) return std::string();

		step_fn(it);
		std::string		last = current;
		while (it != end) {
			// If the iterator didn't move for some reason, that's a bug
			if (it->path().filename() == last) return std::string();
			// Success
			if (is_gif(it->path())) return it->path().string();

			last = it->path().filename().string();
			step_fn(it);
		}
	} catch (std::exception const&) {
	}
	return std::string();
}

}

std::string DirectoryNavigation::previous() {
	if (mCurrentPath.empty()) return std::string();
	return std::string();
}

std::string DirectoryNavigation::next() {
	if (mCurrentPath.empty()) return std::string();

	std::string		next = move_file(mCurrentPath, [](ci::fs::directory_iterator &it){++it;});
	if (next.empty() || next == mCurrentPath) return std::string();
	mCurrentPath = next;
	return mCurrentPath;
}

/**
 * @class cs::FileListNavigation
 */
FileListNavigation::FileListNavigation(const std::vector<std::string>&) {
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
