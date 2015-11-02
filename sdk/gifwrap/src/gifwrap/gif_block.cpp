#include "gif_block.h"

namespace gif {

/**
 * @class gif::Block
 */
size_t Block::readSubBlocks(const std::vector<char> &buffer, size_t position) {
	// Read data blocks, first byte is block size, 0 is the terminator
	uint8_t				block_size = 0;
	while ( (block_size = buffer[position++]) != 0) {
		DataRef			data = std::make_shared<Data>();
		data->mData.reserve(block_size);
		data->mData.insert(data->mData.begin(), buffer.begin()+position, buffer.begin()+position+block_size);
		mSubBlocks.push_back(data);

		position += block_size;
	}
	return position;
}

/**
 * @class gif::GraphicControlExtension
 */
size_t GraphicControlExtension::read(const std::vector<char> &buffer, size_t position) {
	// We are past the introducer and GCE bytes here
	uint8_t				block_size = buffer[position++];
	if (block_size != 4) throw std::runtime_error("GraphicControlExtension has illegal Block Size");

	// fields
	const uint8_t		fields = buffer[position++];
	if ((fields&(1<<0)) != 0) mFlags |= TRANSPARENT_COLOR_F;
	if ((fields&(1<<1)) != 0) mFlags |= USER_INPUT_EXPECTED_F;
	const uint8_t		disposal = ((fields>>2)&0x7);
	if (disposal == 1) mDisposal = Disposal::kDoNotDispose;
	else if (disposal == 2) mDisposal = Disposal::kRestoreToBackgroundColor;
	else if (disposal == 3) mDisposal = Disposal::kRestoreToPrevious;

	// delay time
	const uint8_t		dt0 = buffer[position++],
						dt1 = buffer[position++];
	if (dt0 != 0 || dt1 != 0) {
		int32_t			v = (static_cast<int32_t>(dt1) << 8) | static_cast<int32_t>(dt0);
		mDelay = static_cast<double>(v) / 100.0;
	}

	// transparent color index
	mTransparencyIndex = buffer[position++];

	// terminator
	const uint8_t		term = buffer[position++];
	if (term != 0) throw std::runtime_error("GraphicControlExtension missing block terminator");

	return position;
}

} // namespace gif
