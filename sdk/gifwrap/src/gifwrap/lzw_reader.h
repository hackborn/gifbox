#ifndef GIFWRAP_LZWREADER_H_
#define GIFWRAP_LZWREADER_H_

#include <cstdint>
#include <functional>
#include <vector>

namespace gif {

/**
 * @class gif::LzwReader
 * @brief Decode an LZW buffer, specifically one used for GIFs.
 * @description This implementation was based on the extremely nice one in the Go library:
https://golang.org/src/compress/lzw/reader.go
https://golang.org/src/image/gif/reader.go
 */
class LzwReader {
public:
	using Container = std::vector<char>;
	using Iter = Container::iterator;
	using CIter = Container::const_iterator;

	LzwReader() { }

	void						begin(	const uint8_t code_size,
										const std::function<void(const std::vector<uint8_t>&)> &flush_fn);
	// Decode the sequence, periodically sending the current codes
	// to the flush function assigned in begin().
	// Answer false on error.
	bool						decode(CIter begin, CIter end);

private:
	uint16_t					readCodeLsb(CIter &begin, CIter end, bool &error);
	void						flush();

	std::function<void(const std::vector<uint8_t>&)>
								mFlushFn;
	uint16_t					mClearCode = 0,
								mEndCode = 0,
								mHiCode = 0,
								mOverflow = 0,
								mLast = 0;
	uint32_t					mCodeSize = 0,
								mWidth = 0,
								mNBits = 0,
								mBits = 0,
								mO = 0;
	std::vector<uint8_t>		mSuffix;
	std::vector<uint16_t>		mPrefix;
	std::vector<uint8_t>		mOutput;
	std::vector<uint8_t>		mAnswer;
};

} // namespace gif

#endif
