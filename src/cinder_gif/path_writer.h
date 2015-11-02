#ifndef APP_CINDERGIF_PATHWRITER_H_
#define APP_CINDERGIF_PATHWRITER_H_

#include <gifwrap/gif_file.h>

namespace cs {

/**
 * @class cs::PathWriter
 * @brief Specialize a GIF writer to operate on file paths.
 * @description This is an example of moving all local framework conversion to a writer class.
 * It's unnecessary, but can clean up an API and make the component more reuseable.
 */
class PathWriter : public gif::WriterT<std::string> {
public:
	PathWriter(std::string save_path);

private:
	void					convert(const std::string &src, gif::Bitmap &dst) const;

	using base = WriterT<std::string>;
};

} // namespace cs

#endif
