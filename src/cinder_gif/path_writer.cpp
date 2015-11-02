#include "path_writer.h"

#include <stdexcept>
#include <cinder/ImageIo.h>

namespace cs {

/**
 * @class cs::PathWriter
 */
PathWriter::PathWriter(std::string path)
		: base([this](const std::string &src, gif::Bitmap &dst){convert(src, dst);}, path) {
}

void PathWriter::convert(const std::string &filename, gif::Bitmap &dst) const {
	ci::Surface8u		src(ci::loadImage(filename));
	if (src.getWidth() < 1 || src.getHeight() < 1) throw std::runtime_error("PathWriter could not load source " + filename);
	dst.setTo(src.getWidth(), src.getHeight());
	if (dst.empty()) throw std::runtime_error("PathWriter could not create destination");

	auto				dit = dst.mPixels.begin();
	auto				sit = src.getIter();
	while (sit.line()) {
		while (sit.pixel()) {
			dit->r = sit.r();
			dit->g = sit.g();
			dit->b = sit.b();
			dit->a = sit.a();
			++dit;
		}
	}
}

} // namespace cs
