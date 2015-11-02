#ifndef KT_SERVICE_ALL_FONTSERVICE_H_
#define KT_SERVICE_ALL_FONTSERVICE_H_

#include <string>
#include <unordered_map>
#include <cinder/gl/TextureFont.h>
#include <kt/app/service.h>

namespace kt {

/**
 * @class kt::FontService
 * @brief Store all the fonts.
 */
class FontService : public Service {
public:
	static const std::string&	NAME();
	FontService();

	ci::gl::TextureFontRef		getFont(const std::string &key) const;

private:
	void						loadFonts();

	std::unordered_map<std::string, ci::gl::TextureFontRef>
								mFonts;
};

} // namespace kt

#endif
