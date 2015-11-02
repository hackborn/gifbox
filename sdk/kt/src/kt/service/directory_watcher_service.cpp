#include "directory_watcher_service.h"

#include <algorithm>
#include <kt/app/directory_watcher.h>

namespace kt {

/**
 * @class kt::DirectoryWatcherService
 */
const std::string& DirectoryWatcherService::NAME() {
	static const std::string	N("kt/dirwatch");
	return N;
}

DirectoryWatcherService::DirectoryWatcherService(kt::Cns &cns)
		: UpdateClient(cns)
		, mWaiter(mStop) {
	mStop.store(false);
}

DirectoryWatcherService::~DirectoryWatcherService() {
	stop();
}

void DirectoryWatcherService::update(const kt::UpdateParams&) {
	mWaiter.update(mLocalPaths);
	if (mLocalPaths.empty()) return;
	for (auto it=mLocalPaths.begin(), end=mLocalPaths.end(); it!=end; ++it) {
		for (auto& c : mClients) {
			if (c) c->onChanged(*it);
		}
	}
}

void DirectoryWatcherService::addPath(const std::string& path) {
	try {
		if (!path.empty()) mWaiter.mPath.push_back(path);
	} catch (std::exception&) {
	}
}

void DirectoryWatcherService::start() {
	stop();
	mStop.store(false);
	try {
		mThread = std::thread([this](){mWaiter.run();});
	} catch (std::exception&) {
	}
}

void DirectoryWatcherService::stop() {
	mStop.store(true);
	wakeup();
	try {
		mThread.join();
	} catch (std::exception&) {
	}
}

void DirectoryWatcherService::addClient(DirectoryWatcher &c) {
	try {
		mClients.push_back(&c);
	} catch (std::exception const&) {
	}
}

void DirectoryWatcherService::removeClient(DirectoryWatcher *c) {
	if (!c) return;
	try {
		mClients.erase(std::remove(mClients.begin(), mClients.end(), c), mClients.end());
	} catch (std::exception const&) {
	}
}

/**
 * @class kt::DirectoryWatcherService::Waiter
 * @brief Handle waiting on directory changes and sending notices.
 */
DirectoryWatcherService::Waiter::Waiter(const std::atomic_bool& stop)
		: mStop(stop) {
}

void DirectoryWatcherService::Waiter::update(std::vector<std::string> &paths) {
	paths.clear();
	{
		std::lock_guard<std::mutex> lock(mLock);
		paths.swap(mChangedPaths);
	}
}

bool DirectoryWatcherService::Waiter::isStopped() {
	return mStop.load();
}

bool DirectoryWatcherService::Waiter::onChanged(const std::string& path) {
	std::lock_guard<std::mutex> lock(mLock);
	if (std::find(mChangedPaths.begin(), mChangedPaths.end(), path) == mChangedPaths.end()) {
		mChangedPaths.push_back(path);
	}
	return true;
}

} // namespace ds