#ifndef KT_VIEW_VIEWDEFS_H_
#define KT_VIEW_VIEWDEFS_H_

#include <memory>

namespace kt {
namespace view {
class RootCfg;
using RootCfgRef = std::shared_ptr<RootCfg>;

// Standard visual states. The actual strings for all application states will
// be prefixed with "kt".
// Note: defined in view.cpp
	// Default at-rest state.
extern const std::string&	REST_STATE;
	// View is being pressed down.
extern const std::string&	DOWN_STATE;

/**
 * @class kt::view::DrawParams
 */
class DrawParams {
public:
	DrawParams() { }
};

/**
 * @class kt::view::RootCfg
 * @brief Configuration of the root.
 */
class RootCfg {
public:
	RootCfg() { }
	RootCfg(const bool invert_y) : mInvertY(invert_y) { }

	bool			mInvertY = false;
};

} // namespace view
} // namespace kt

#endif
