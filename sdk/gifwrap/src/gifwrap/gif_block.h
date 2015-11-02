#ifndef GIFWRAP_GIFBLOCK_H_
#define GIFWRAP_GIFBLOCK_H_

#include <memory>
#include <string>
#include <vector>

namespace gif {
class Block;
using BlockRef = std::shared_ptr<Block>;
class Data;
using DataRef = std::shared_ptr<Data>;
class GraphicControlExtension;
using GraphicControlExtensionRef = std::shared_ptr<GraphicControlExtension>;

/**
 * @class gif::Data
 * @brief Generic byte stream.
 */
class Data {
public:
	Data() { }

	std::vector<char>	mData;
};

/**
 * @class gif::Block
 * @brief Superclass for all block types.
 */
class Block {
public:
	Block() { }
	virtual ~Block() { }

	// Generic utility to read blocks
	size_t					readSubBlocks(const std::vector<char> &buffer, size_t position);

	std::vector<DataRef>	mSubBlocks;
};

/**
 * @class gif::GraphicControlExtension
 * @brief Per the GIF spec.
 */
class GraphicControlExtension : public Block {
public:
	enum class Disposal			{ kUnspecified, kDoNotDispose, kRestoreToBackgroundColor, kRestoreToPrevious };
	static const uint32_t		TRANSPARENT_COLOR_F = (1<<0);
	static const uint32_t		USER_INPUT_EXPECTED_F = (1<<1);

	GraphicControlExtension() { }

	bool					hasTransparentColor() const { return (mFlags&TRANSPARENT_COLOR_F) != 0; }

	size_t					read(const std::vector<char> &buffer, size_t position);

	uint32_t				mFlags = 0;
	Disposal				mDisposal = Disposal::kUnspecified;
	uint8_t					mTransparencyIndex = 0;
	double					mDelay = 0.0;
};

} // namespace gif

#endif
