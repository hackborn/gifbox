#include "directory_watcher.h"

#include <kt/service/directory_watcher_service.h>
#include "cns.h"

namespace kt {

/**
 * @class kt::DirectoryWatcher
 */
DirectoryWatcher::DirectoryWatcher(kt::Cns &cns, const std::function<void(const std::string&)> &fn)
		: mService(cns.getService<DirectoryWatcherService>(DirectoryWatcherService::NAME()))
		, mOnChangeFn(fn) {
	mService.addClient(*this);
}

DirectoryWatcher::~DirectoryWatcher() {
	mService.removeClient(this);
}

void DirectoryWatcher::setOnChangeFn(const std::function<void(const std::string&)> &fn) {
	mOnChangeFn = fn;
}

void DirectoryWatcher::onChanged(const std::string &path) {
	if (mOnChangeFn) mOnChangeFn(path);
}

} // namespace kt
