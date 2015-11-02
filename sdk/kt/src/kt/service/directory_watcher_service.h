#ifndef KT_SERVICE_DIRECTORYWATCHERSERVICE_H_
#define KT_SERVICE_DIRECTORYWATCHERSERVICE_H_

#include <atomic>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>
#include <kt/app/service.h>
#include <kt/app/update.h>

namespace kt {
class DirectoryWatcher;

/**
 * @class kt::DirectoryWatcherService
 */
class DirectoryWatcherService : public Service
							  , public UpdateClient {
public:
	static const std::string&	NAME();
	DirectoryWatcherService(kt::Cns&);
	~DirectoryWatcherService();

	void						update(const kt::UpdateParams&) override;

	// After you''re done changing, you'll need to call start()
	// for changes to take effect.
	void						addPath(const std::string& path);

	void						start();
	void						stop();

	void						addClient(DirectoryWatcher&);
	void						removeClient(DirectoryWatcher*);

private:
	std::vector<std::string>	mLocalPaths;
	std::vector<DirectoryWatcher*>
								mClients;

class Waiter {
public:
	// Directories I'm watching
	std::vector<std::string>	mPath;

public:
	Waiter(const std::atomic_bool&);

	// The platform implementation is responsible for suppling a run().
	void						run();
	void						update(std::vector<std::string> &paths);

	bool						isStopped();
	bool						onChanged(const std::string& path);

private:
	const std::atomic_bool&		mStop;

	// Shared between worker and main threads.
	std::mutex					mLock;
	std::vector<std::string>	mChangedPaths;
};

private:
	std::atomic_bool			mStop;
	std::thread					mThread;
	Waiter						mWaiter;

	// The platform implementation is responsible for waking up the thread.
	void						wakeup();
};

} // namespace kt

#endif