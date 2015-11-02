#ifndef KT_APP_DIRECTORYWATCHER_H_
#define KT_APP_DIRECTORYWATCHER_H_

#include <functional>
#include <string>

namespace kt {
class Cns;
class DirectoryWatcherService;

/**
 * @class kt::DirectoryWatcher
 * @brief Subscribe to and receive notification about directory changes.
 */
class DirectoryWatcher {
public:
	DirectoryWatcher() = delete;
	DirectoryWatcher(const DirectoryWatcher&) = delete;
	explicit DirectoryWatcher(	kt::Cns&,
								const std::function<void(const std::string&)>& = nullptr);
	virtual ~DirectoryWatcher();

	void						setOnChangeFn(const std::function<void(const std::string&)>&);

	void						onChanged(const std::string &path);

private:
	DirectoryWatcherService&	mService;
	std::function<void(const std::string&)>
								mOnChangeFn;
};

} // namespace kt

#endif
