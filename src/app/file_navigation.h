#ifndef APP_APP_FILENAVIGATION_H_
#define APP_APP_FILENAVIGATION_H_

#include <memory>
#include <vector>
#include <string>

namespace cs {
class FileNavigation;
using FileNavigationRef = std::shared_ptr<FileNavigation>;

/**
 * @class cs::FileNavigation
 * @brief Abstract oject for stepping through files.
 */
class FileNavigation {
public:
	FileNavigation() { }

	virtual bool			hasPrevious() const = 0;
	virtual bool			hasNext() const = 0;

	virtual std::string		previous() = 0;
	virtual std::string		next() = 0;
};

/**
 * @class cs::DirectoryNavigation
 * @brief Step through a directory.
 */
class DirectoryNavigation : public FileNavigation {
public:
	DirectoryNavigation() = delete;
	DirectoryNavigation(const std::string &starting_path);

	bool					hasPrevious() const override;
	bool					hasNext() const override;

	std::string				previous() override;
	std::string				next() override;

private:
	std::string				mCurrentPath;
};

/**
 * @class cs::FileListNavigation
 * @brief Step through a predetermined list of files.
 */
class FileListNavigation : public FileNavigation {
public:
	FileListNavigation() = delete;
	FileListNavigation(const std::vector<std::string>&);

	bool					hasPrevious() const override;
	bool					hasNext() const override;

	std::string				previous() override;
	std::string				next() override;
};

} // namespace cs

#endif