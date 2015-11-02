#include "font_service.h"

#include <cinder/Filesystem.h>
#include <cinder/Xml.h>
#include <kt/app/environment.h>
#include <kt/app/settings.h>

namespace kt {

namespace {
const std::string			DEFAULT_FN("default");
void						load_fonts(	const std::string &unexpanded_root_path,
										std::unordered_map<std::string, ci::gl::TextureFontRef> &out);
void						load_xml(	const ci::XmlTree&,
										std::unordered_map<std::string, ci::gl::TextureFontRef>&);
void						load_font(	const ci::XmlTree&,
										std::unordered_map<std::string, ci::gl::TextureFontRef>&);
}

/**
 * \class kt::FontService
 */
const std::string& FontService::NAME() {
	static const std::string	N("kt/fonts");
	return N;
}

FontService::FontService() {
	// Create a default
	// XXX Should verify the font actually exists
// Hmm... I think create no fonts by default, for clients that don't want them
//	ci::Font				font("Times New Roman", 24);
//	mFonts[DEFAULT_FN] = ci::gl::TextureFont::create(font);

	loadFonts();

	// Handy little font printer
#if 0
	const std::vector<std::string>& list(ci::Font::getNames());
	for (const auto& it : list) {
		std::cout << "Font=" << it << std::endl;
	}
#endif
}

ci::gl::TextureFontRef FontService::getFont(const std::string &name) const {
	auto f = mFonts.find(name);
	if (f == mFonts.end()) f = mFonts.find(DEFAULT_FN);
	if (f == mFonts.end()) return ci::gl::TextureFontRef();
	return f->second;
}

void FontService::loadFonts() {
	try {
		load_fonts("$(DATA)/settings", mFonts);
		load_fonts("$(LOCAL)/settings", mFonts);
	} catch (std::exception const&) {
	}
}

namespace {

void load_fonts(const std::string &unexpanded_root_path, std::unordered_map<std::string, ci::gl::TextureFontRef> &out) {
	std::string					root_path(kt::env::expand(unexpanded_root_path));
	ci::fs::directory_iterator	end_iter;
	if (!ci::fs::exists(root_path) || !ci::fs::is_directory(root_path)) return;

	for (ci::fs::directory_iterator it(root_path); it != end_iter; ++it) {
		try {
			// Valid file?
			if (!ci::fs::is_regular_file(it->status())) continue;
			const ci::fs::path&			path = it->path();
			// Valid name?
			const std::string			name(path.stem().string());
			if (name.empty()) continue;
			// XML file?
			std::string					ext(path.extension().string());
			std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
			if (ext != ".xml") continue;
			// Valid font file?
			ci::XmlTree					xml(ci::loadFile(path));
			for (auto it=xml.begin(), end=xml.end(); it!=end; ++it) {
				if (it->getTag() == "kt_fonts") {
					load_xml(*it, out);
				}
			}
		} catch (std::exception const&) {
		}
    }
}

void						load_xml(	const ci::XmlTree &xml,
										std::unordered_map<std::string, ci::gl::TextureFontRef> &map) {
	for (auto it=xml.begin(), end=xml.end(); it!=end; ++it) {
		if (it->getTag() == "font") {
			try {
				load_font(*it, map);
			} catch (std::exception const&) {
			}
		}
	}
}

void						load_font(	const ci::XmlTree &xml,
										std::unordered_map<std::string, ci::gl::TextureFontRef> &map) {
	std::string		key(xml.getAttributeValue<std::string>("key", std::string()));
	std::string		font(xml.getAttributeValue<std::string>("font", std::string()));
	float			size(xml.getAttributeValue<float>("size", -1.0f));
	if (!key.empty() && !font.empty() && size > 0.0f) {
		// XXX Should verify the font actually exists
		ci::Font	f(font, size);
		map[key] = ci::gl::TextureFont::create(f);
	}
}

}

} // namespace kt
