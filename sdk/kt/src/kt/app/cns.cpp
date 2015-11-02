#include "cns.h"

#include <algorithm>
#include <set>
#include <cinder/Filesystem.h>
#include <cinder/Xml.h>
#include <kt/app/environment.h>

namespace kt {

namespace {
const kt::Settings		EMPTY_SETTINGS;
void					load_settings_directory(const std::string &unexpanded_root_path,
												const bool is_local,
												std::unordered_map<std::string, kt::Settings> &out,
												std::set<std::string> *out_paths);
}

/**
 * @class kt::Cns
 */
Cns::Cns()
		: mEnvInitialized(kt::Environment::initialize()) {
	loadSettings();
}

Cns::~Cns() {
}

const kt::Settings& Cns::getSettings(const std::string &name) const {
	try {
		const auto it(mSettings.find(name));
		if (it != mSettings.end()) return it->second;
	} catch (std::exception const&) {
	}
	return EMPTY_SETTINGS;
}

void Cns::addService(const std::string &name, std::unique_ptr<Service> s) {
	if (!s) throw std::runtime_error("Cns adding nullptr service " + name);
	mServices[name] = std::move(s);
}

void Cns::loadSettings() {
	// Default system settings. You can override these by making a "system.xml" file
	// but otherwise will use these.
	{
		kt::Settings&			s(mSettings["system"]);
//		s.setDoubleOrThrow("tap_threshold", 5.0);
	}

	std::set<std::string>		settings_directories;

	load_settings_directory("$(DATA)/settings", false, mSettings, &settings_directories);

	// Load and apply additional settings.
	try {
		std::string				local_path(mSettings["system"].getString("app:local"));
		if (!local_path.empty()) {
			kt::env::initializeLocal(kt::env::expand(local_path));
			local_path += "/settings";
			load_settings_directory(local_path, true, mSettings, &settings_directories);
		}
	} catch (std::exception const&) {
	}

	// Store these directories, in case the app wants to create a watcher for them.
	for (const auto& e : settings_directories) mSettingsDirectories.push_back(e);
}

void Cns::updateSettings(const std::string &path) {
	if (std::find(mSettingsDirectories.begin(), mSettingsDirectories.end(), path) == mSettingsDirectories.end()) return;
	load_settings_directory(path, false, mSettings, nullptr);
}

namespace {

void load_settings_directory(	const std::string &unexpanded_root_path,
								const bool is_local,
								std::unordered_map<std::string, kt::Settings> &out,
								std::set<std::string> *out_paths) {
	// Eventually C++ will have filesystem support.
	namespace csfs = ci::fs;
	std::string					root_path(kt::env::expand(unexpanded_root_path));
	csfs::directory_iterator	end_iter;
	const csfs::path			root_as_path(root_path);
	if (!csfs::exists(root_path) || !csfs::is_directory(root_as_path)) return;

	for (csfs::directory_iterator it(root_path); it != end_iter; ++it) {
		try {
			// Valid file?
			if (!csfs::is_regular_file(it->status())) continue;
			const csfs::path&			path = it->path();
			// Valid name?
			const std::string			name(path.stem().string());
			if (name.empty()) continue;
			// XML file?
			std::string					ext(path.extension().string());
			std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
			if (ext != ".xml") continue;
			// Valid settings file?
			const std::string			needed_root("settings");
			bool						loaded = false;
			if (kt::Settings::getRootName(path.string()) == needed_root) {
				kt::Settings&			found(out[name]);
				found.load(path.string(), is_local, needed_root);
				loaded = true;
			}
			if (out_paths && loaded) out_paths->insert(root_path);
		} catch (std::exception const &ex) {
			std::cout << "Cns::loadSettings() error on load=" << ex.what() << std::endl;
			assert(false);
		}
    }
}

}

} // namespace kt
