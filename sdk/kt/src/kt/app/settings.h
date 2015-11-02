#ifndef KT_APP_SETTINGS_H_
#define KT_APP_SETTINGS_H_

// This is annoyingly needed for zenxml. I hate dragging this into what
// will end up being the entire project, but it's not worth breaking
// zen into a separate file right now.
#pragma warning(disable: 4996)
#include <cstdint>
#include <functional>
#include <string>
#include <memory>
#include <unordered_map>
#include <cinder/Color.h>
#include <cinder/Vector.h>

namespace kt {

/**
 * @class kt::Settings
 */
class Settings {
public:
	// Utility to see get the file root. Here currently because XML loading
	// is entirely local to the Settings
	static std::string			getRootName(const std::string &path);
	Settings();

	// This will only replace or add data, it can never remove it.
	// If local is true, then cache the path and root info so I resave.
	void						load(const std::string &path, const bool local, const std::string &root = "settings");
	void						save(const std::string &path, const std::string &root = "settings") const;
	// Save using my local info
	void						saveLocal() const;

	// Answer references to the desired object. This lets the framework reload new
	// * If you request a setting that doesn't exist, then is later loaded into the settings
	// object, you will not see the newly-created setting. This might get fixed at some point,
	// but doing so would mean the class would need to be completely thread safe.
	const bool&					getBool(const std::string &key) const;
	const float&				getFloat(const std::string &key) const;
	const std::int32_t&			getInt(const std::string &key) const;
	const std::size_t&			getSize(const std::string &key) const;
	const std::string&			getString(const std::string &key) const;
	const glm::vec2&			getVec2(const std::string &key) const;
	const glm::vec3&			getVec3(const std::string &key) const;
	const ci::ColorAf&			getColorA(const std::string &key) const;

	void						setBoolOrThrow(const std::string &key, const bool v);
	void						setFloatOrThrow(const std::string &key, const float v);
	void						setIntOrThrow(const std::string &key, const std::int32_t v);
	void						setStringOrThrow(const std::string &key, const std::string &v);
	void						setVec2OrThrow(const std::string &key, const glm::vec2 &v);
	void						setVec3OrThrow(const std::string &key, const glm::vec3 &v);

	// Keys
	void						forEachString(const std::function<void(const std::string&, const std::string&)>&) const;

private:
	template <typename T>
	class Container {
	public:
		Container()				{ }

		const T&				get(const std::string &key) const {
			auto f = mData.find(key);
			if (f != mData.end()) return *(f->second.get());
			return mDne;
		}

		std::unordered_map<std::string, std::shared_ptr<T>>
								mData;
		// Default does not exist value.
		T						mDne;
	};
	Container<bool>				mBool;
	Container<float>			mFloat;
	Container<std::int32_t>		mInt;
	Container<std::string>		mString;
	Container<std::size_t>		mSize;
	Container<glm::vec2>		mVec2;
	Container<glm::vec3>		mVec3;
	Container<ci::ColorAf>		mColorA;
	// Target file to save to.
	std::string					mLocalPath;
	std::string					mLocalRoot;
};

} // namespace kt

#endif
