#include "environment.h"

#include <cinder/Cinder.h>

#include <algorithm>
#include <assert.h>
#include <map>

#ifdef CINDER_MSW
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif
#include <windows.h>
#include <filesystem>
#include <Shlobj.h>
#endif

#include "kt_string.h"

namespace kt {

namespace {
std::string				get_documents_path();
std::string				get_saved_games_path();

// Mapping of environment variable to expanded value.
std::map<std::string, std::string>	VARS;
}

/**
 * @class kt::Environment
 */
std::string Environment::expand(std::string s) {
	try {
		for (const auto& kv : VARS) {
			kt::replace_substr(kv.first, kv.second, s);
		}
	} catch (std::exception const&) {
	}
	return s;
}

bool Environment::initialize() {
	// Use VS experimental C++ filesystem support
	namespace fs = std::tr2::sys;
	fs::path			app_path = fs::initial_path<fs::path>();
	// Find the local data folder
	{
		fs::path		p(app_path);
		while (!p.empty()) {
			fs::path	dp(p);
			dp /= "data";
			if (fs::exists(dp)) {
				VARS["$(DATA)"] = dp.string();
				break;
			}
			p = p.parent_path();
		}
	}
	const std::string	_app_path = app_path.string();
	const std::string	_doc_path(get_documents_path());
	const std::string	_sg_path(get_saved_games_path());

	VARS["$(APP)"] = _app_path;
	VARS["$(DOCUMENTS)"] = _doc_path;
	VARS["$(SAVED_GAMES)"] = _sg_path;

	if (_app_path.empty()) throw std::runtime_error("kt::Environment::initialize() can't create $(APP)");
	if (_doc_path.empty()) throw std::runtime_error("kt::Environment::initialize() can't create $(DOCUMENTS)");
	if (_sg_path.empty()) throw std::runtime_error("kt::Environment::initialize() can't create $(SAVED_GAMES)");
	return true;
}

void Environment::initializeLocal(const std::string &lp) {
	VARS["$(LOCAL)"] = lp;
}

namespace {

std::string				get_documents_path() {
#ifdef CINDER_MSW
	std::string			ans;
	PWSTR				dir = 0;
	HRESULT				hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &dir);
	if (SUCCEEDED(hr)) {
		if (dir) {
			std::wstring	wstr(dir);
			ans = str_from_wstr(wstr);
		}
		CoTaskMemFree(dir);
	}
	return ans;
#else
	assert(false);
#endif
}

std::string				get_saved_games_path() {
#ifdef CINDER_MSW
	std::string			ans;
	PWSTR				dir = 0;
	HRESULT				hr = SHGetKnownFolderPath(FOLDERID_SavedGames, 0, NULL, &dir);
	if (SUCCEEDED(hr)) {
		if (dir) {
			std::wstring	wstr(dir);
			ans = str_from_wstr(wstr);
		}
		CoTaskMemFree(dir);
	}
	return ans;
#else
	assert(false);
#endif
}

}

} // namespace kt