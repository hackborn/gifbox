#ifndef GIFWRAP_LZWWRITER_H_
#define GIFWRAP_LZWWRITER_H_

#include <cstdint>
#include <fstream>
#include <functional>
#include <vector>
#include <unordered_map>

namespace gif {

/**
 * @class gif::LzwWriter
 * @brief Encode an LZW buffer, specifically one used for GIFs.
 * @description This implementation was based on the extremely nice one in the Go library:
https://golang.org/src/compress/lzw/writer.go
https://golang.org/src/image/gif/writer.go
 */
class LzwWriter {
public:
	LzwWriter() { }

	void						begin(	const uint8_t code_size,
										const std::function<void(const std::vector<uint8_t>&)> &flush_fn);
	void						encode(const std::vector<uint8_t>&);

private:
	void						close();
	enum class IncError			{ kNone, kOutOfCodes, kWriteFailed };
	inline uint32_t				clearCode() const { return static_cast<uint32_t>(1) << mCodeSize; }
	void						writeCodeLsb(const uint32_t code);
	IncError					incHi();

	std::function<void(const std::vector<uint8_t>&)>
								mFlushFn;

	uint32_t					mCodeSize = 0,
								mWidth = 0,
								mNBits = 0,
								mBits = 0,
								mHi = 0,
								mOverflow = 0,
								mSavedCode = 0;
	std::unordered_map<uint32_t, uint32_t>
								mTable;
	std::vector<uint8_t>		mOutput;
};

/**
 * @class gif::WriterBuffer
 * @brief Utility to handle writing blocks of bytes in the standard GIF format.
 */
class WriterBuffer {
public:
	WriterBuffer() = delete;
	WriterBuffer(const WriterBuffer&) = delete;
	WriterBuffer(std::ostream &s) : mStream(s) { }

	// Clear my buffer without writing anything
	void						clear() { mBuffer.clear(); }
	void						write(const std::vector<uint8_t>&);
	// Force writing my current state, even if it's not large enough for a block,
	// and write the block terminator.
	void						terminate();

private:
	// Write all completed blocks.
	void						writeFullBlocks();
	void						writeBlock(const size_t size);

	std::ostream&				mStream;
	std::vector<uint8_t>		mBuffer;
	size_t						mBlockSize = 255;
};

} // namespace gif

#endif
