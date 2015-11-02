#ifndef KT_APP_CNS_H_
#define KT_APP_CNS_H_

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include "service.h"
#include "settings.h"

namespace kt {
class App;

/**
 * @class kt::Cns
 * @brief Central nervous system for the framework, a global object
 * that gets passed to every part of the system.
 */
class Cns {
public:
	Cns();
	virtual ~Cns();

	const kt::Settings&		getSettings(const std::string &name) const;
	// Get the desired service, optionally creating it if it doesn't exist.
	template<typename T>
	T&						getService(const std::function<T*(void)>& = nullptr);
	// Use this when you have multiple, separately-named instances of the same service type.
	template<typename T>
	T&						getService(const std::string &name, const std::function<T*(void)>& = nullptr);

	void					addService(const std::string &name, std::unique_ptr<Service>);

private:
	// Automatically load all settings in my data/settings folder. To be a settings
	// file, it must be an xml with a root tag of rf_settings.
	void					loadSettings();
	void					updateSettings(const std::string &path);

	friend class kt::App;
	const bool				mEnvInitialized;
	std::unordered_map<std::string, kt::Settings>
							mSettings;
	std::unordered_map<std::string, std::unique_ptr<Service>>
							mServices;
	// All the directories that contain settings. Used for when settings are live.
	std::vector<std::string> mSettingsDirectories;
};

template<typename T>
T& Cns::getService(const std::function<T*(void)> &fn) {
	return getService<T>(T::NAME(), fn);
}

template<typename T>
T& Cns::getService(const std::string &name, const std::function<T*(void)> &fn) {
	auto f=mServices.find(name);
	if (f == mServices.end()) {
		T*								t(fn ? fn() : nullptr);
		if (t) {
			std::unique_ptr<Service>	up(t);
			mServices[name] = std::move(up);
			return *t;
		}
		throw std::runtime_error("Can't find service " + name);
	}
	if (!(f->second)) throw std::runtime_error("Service " + name + " is nullptr");
	return static_cast<T&>(*(f->second.get()));
}

} // namespace kt

#endif
