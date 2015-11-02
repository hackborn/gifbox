#include "settings.h"

#include <filesystem>
#include <cinder/Xml.h>
#include "environment.h"

namespace kt {

namespace {

class Loader {
public:
	Loader() = delete;
	Loader(const Loader&) = delete;
	Loader(	const ci::XmlTree&,
			const std::string &root,
			std::unordered_map<std::string, std::shared_ptr<bool>>&,
			std::unordered_map<std::string, std::shared_ptr<float>>&,
			std::unordered_map<std::string, std::shared_ptr<std::int32_t>>&,
			std::unordered_map<std::string, std::shared_ptr<std::size_t>>&,
			std::unordered_map<std::string, std::shared_ptr<std::string>>&,
			std::unordered_map<std::string, std::shared_ptr<glm::vec2>>&,
			std::unordered_map<std::string, std::shared_ptr<glm::vec3>>&,
			std::unordered_map<std::string, std::shared_ptr<ci::ColorAf>>&);

private:
	void					loadElement(std::string parent, const ci::XmlTree&);
	void					addV(const std::string &parent, const std::string &v);

	const ci::XmlTree&		mDoc;
	std::unordered_map<std::string, std::shared_ptr<bool>>&
							mBool;
	std::unordered_map<std::string, std::shared_ptr<float>>&
							mFloat;
	std::unordered_map<std::string, std::shared_ptr<std::int32_t>>&
							mInt;
	std::unordered_map<std::string, std::shared_ptr<std::size_t>>&
							mSize;
	std::unordered_map<std::string, std::shared_ptr<std::string>>&
							mString;
	std::unordered_map<std::string, std::shared_ptr<glm::vec2>>&
							mVec2;
	std::unordered_map<std::string, std::shared_ptr<glm::vec3>>&
							mVec3;
	std::unordered_map<std::string, std::shared_ptr<ci::ColorAf>>&
							mColorA;
};

template <typename T>
void			set(const std::string &key, const T &v,
					std::unordered_map<std::string, std::shared_ptr<T>> &map) {
	auto f = map.find(key);
	if (f != map.end()) {
		*(f->second.get()) = v;
	} else {
		map[key] = std::move(std::shared_ptr<T>(new T(v)));
	}
}

}

/**
 * @class kt::Settings
 */
std::string Settings::getRootName(const std::string &path) {
	try {
		ci::XmlTree				xml(ci::loadFile(kt::env::expand(path)));
		// Hmmm... root is 1 level deep?
		const auto&				children(xml.getChildren());
		if (children.empty()) return std::string();
		if (!children.front()) return std::string();
		return children.front()->getTag();
//		return xml.getTag();
	} catch (std::exception const&) {
	}
	return std::string();
}

Settings::Settings() {
	mBool.mDne = false;
	mFloat.mDne = 0.0f;
	mInt.mDne = 0;
	mSize.mDne = 0;
	mVec2.mDne =  glm::vec2(0.0f, 0.0f);
	mVec3.mDne = glm::vec3(0.0f, 0.0f, 0.0f);
	mColorA.mDne = ci::ColorAf(0.0f, 0.0f, 0.0f, 0.0f);
}

void Settings::load(const std::string &_path, const bool local, const std::string &root) {
	try {
		const std::string	path(kt::env::expand(_path));
		if (local) {
			mLocalPath = path;
			mLocalRoot = root;
		}
		// Use VS experimental C++ filesystem support
		namespace fs = std::tr2::sys;

		fs::path			p(path);
		if (!fs::exists(p)) return;

		ci::XmlTree				xml(ci::loadFile(kt::env::expand(path)));
		Loader					loader(xml, root, mBool.mData, mFloat.mData, mInt.mData, mSize.mData, mString.mData, mVec2.mData, mVec3.mData, mColorA.mData);
	} catch (std::exception const&) {
	}
}

void Settings::save(const std::string &path, const std::string &root) const {
	try {
	} catch (std::exception const&) {
	}
}

void Settings::saveLocal() const {
	if (mLocalPath.empty()) {
		return;
	}
	if (mLocalRoot.empty()) {
		return;
	}
	save(mLocalPath, mLocalRoot);
}

const bool& Settings::getBool(const std::string &key) const {
	return mBool.get(key);
}

const float& Settings::getFloat(const std::string &key) const {
	return mFloat.get(key);
}

const std::int32_t& Settings::getInt(const std::string &key) const {
	return mInt.get(key);
}

const std::size_t& Settings::getSize(const std::string &key) const {
	return mSize.get(key);
}

const std::string& Settings::getString(const std::string &key) const {
	return mString.get(key);
}

const glm::vec2& Settings::getVec2(const std::string &key) const {
	return mVec2.get(key);
}

const glm::vec3& Settings::getVec3(const std::string &key) const {
	return mVec3.get(key);
}

const ci::ColorAf& Settings::getColorA(const std::string &key) const {
	return mColorA.get(key);
}

void Settings::setBoolOrThrow(const std::string &key, const bool v) {
	if (key.empty()) return;
	set<bool>(key, v, mBool.mData);
}

void Settings::setFloatOrThrow(const std::string &key, const float v) {
	if (key.empty()) return;
	set<float>(key, v, mFloat.mData);
}

void Settings::setIntOrThrow(const std::string &key, const std::int32_t v) {
	if (key.empty()) return;
	set<std::int32_t>(key, v, mInt.mData);
}

void Settings::setStringOrThrow(const std::string &key, const std::string &v) {
	if (key.empty()) return;
	set<std::string>(key, v, mString.mData);
}

void Settings::setVec2OrThrow(const std::string &key, const glm::vec2 &v) {
	if (key.empty()) return;
	set<glm::vec2>(key, v, mVec2.mData);
}

void Settings::setVec3OrThrow(const std::string &key, const glm::vec3 &v) {
	if (key.empty()) return;
	set<glm::vec3>(key, v, mVec3.mData);
}

void Settings::forEachString(const std::function<void(const std::string&, const std::string&)> &fn) const {
	if (!fn) return;
	for (const auto& kv : mString.mData) {
		if (kv.second) {
			fn(kv.first, *(kv.second.get()));
		}
	}
}

namespace {

Loader::Loader(	const ci::XmlTree &d,
				const std::string &root_name,
				std::unordered_map<std::string, std::shared_ptr<bool>> &b,
				std::unordered_map<std::string, std::shared_ptr<float>> &f,
				std::unordered_map<std::string, std::shared_ptr<std::int32_t>> &i,
				std::unordered_map<std::string, std::shared_ptr<std::size_t>> &size,
				std::unordered_map<std::string, std::shared_ptr<std::string>> &s,
				std::unordered_map<std::string, std::shared_ptr<glm::vec2>> &v2,
				std::unordered_map<std::string, std::shared_ptr<glm::vec3>> &v3,
				std::unordered_map<std::string, std::shared_ptr<ci::ColorAf>> &ca)
		: mDoc(d)
		, mBool(b)
		, mFloat(f)
		, mInt(i)
		, mSize(size)
		, mString(s)
		, mVec2(v2)
		, mVec3(v3)
		, mColorA(ca) {
	for (auto it = d.begin(), end=d.end(); it != end; ++it) {
		if (it->getTag() == root_name) {
			for (auto it2 = it->begin(), end2 = it->end(); it2 != end2; ++it2) {
				loadElement(std::string(), *it2);
			}
			return;
		}
	}
}

namespace {

float get_color(const std::string &v) {
	return std::stof(v) / 255.0f;
}

}

void Loader::loadElement(std::string parent, const ci::XmlTree &xml) {
	if (!parent.empty()) parent += ":";
	parent += xml.getTag();

	// Add any named attributes, along with their index lookups.
	glm::vec3		pt3(0.0f, 0.0f, 0.0f);
	ci::ColorAf		rgba(0.0f, 0.0f, 0.0f, 1.0f);
	bool			has_pt(false), has_rgba(false);
	for (auto it=xml.getAttributes().cbegin(), end=xml.getAttributes().cend(); it!=end; ++it) {
		if (it->getName() == "v") { addV(parent, it->getValue());
		// pt
		} else if (it->getName() == "x") { has_pt = true; pt3.x = it->getValue<float>();
		} else if (it->getName() == "y") { has_pt = true; pt3.y = it->getValue<float>();
		} else if (it->getName() == "z") { has_pt = true; pt3.z = it->getValue<float>();
		// Alternative for point, RGB
		} else if (it->getName() == "r") { has_rgba = true; rgba.r = get_color(it->getValue());
		} else if (it->getName() == "g") { has_rgba = true; rgba.g = get_color(it->getValue());
		} else if (it->getName() == "b") { has_rgba = true; rgba.b = get_color(it->getValue());
		} else if (it->getName() == "a") { has_rgba = true; rgba.a = get_color(it->getValue());
		}
	}
	if (has_pt) {
		set<glm::vec2>(parent, glm::vec2(pt3.x, pt3.y), mVec2);
		set<glm::vec3>(parent, pt3, mVec3);
	}
	if (has_rgba) {
		set<ci::ColorAf>(parent, rgba, mColorA);
	}

	// Add child elements.
	for (auto it = xml.begin(), end = xml.end(); it != end; ++it) {
		loadElement(parent, *it);
	}
}

void Loader::addV(const std::string &parent, const std::string &v) {
	if (parent.empty() || v.empty()) return;

	bool					bv(false);
	float					fv(0);
	int32_t					iv(0);
	size_t					sv(0);
	if (v[0] == 't' || v[0] == 'T') bv = true;
	try { fv = std::stof(v); } catch (std::exception const&) { }
	try { iv = std::stoi(v); } catch (std::exception const&) { }
	try { sv = std::stoul(v); } catch (std::exception const&) { }

	set<bool>(parent, bv, mBool);
	set<float>(parent, fv, mFloat);
	set<int32_t>(parent, iv, mInt);
	set<size_t>(parent, sv, mSize);
	set<std::string>(parent, v, mString);
}

}

} // namespace ci