#ifndef KT_APP_ENVIRONMENT_H_
#define KT_APP_ENVIRONMENT_H_

#include <string>

namespace kt {

/**
 * @class kt::Environment
 */
class Environment {
public:
	// Expand variables in a string.
	// $(DATA) = path to app data folder
	// $(APP) = path to folder that contains the application
	// $(DOCUMENTS) = path to documents folder
	// $(LOCAL) = path to local folder (for app settings, logs, etc.)
	// $(SAVED_GAMES) = path to saved games folder
	static std::string			expand(std::string);

	Environment() = delete;
	Environment(const Environment&) = delete;

private:
	friend class App;
	friend class Cns;
	// Should only happen via the framework, before the app is constructed.
	static bool					initialize();
	static void					initializeLocal(const std::string&);
};

// Convenience shortcut
using env = kt::Environment;

} // namespace kt

#endif
