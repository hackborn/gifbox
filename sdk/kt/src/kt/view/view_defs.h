#ifndef KT_VIEW_VIEWDEFS_H_
#define KT_VIEW_VIEWDEFS_H_

#include <memory>

namespace kt {
namespace view {
class RootCfg;
using RootCfgRef = std::shared_ptr<RootCfg>;

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
