#include "lzw_reader.h"

#include <iostream>
#include <stdexcept>

namespace gif {

namespace {
const uint16_t			DECODER_INVALID = 0xffff;
// In bits
const uint16_t			MAX_WIDTH = 12;
const uint32_t			FLUSH_BUFFER = 1 << MAX_WIDTH;
}

/**
 * @class gif::LzwReader
 */
void LzwReader::begin(	const uint8_t code_size,
						const std::function<void(const std::vector<uint8_t>&)> &flush_fn) {
	if (code_size < 2 || code_size > 8) {
		throw std::runtime_error("Code size invalid");
	}

	mFlushFn = flush_fn;
	mCodeSize = code_size;
	mClearCode = static_cast<uint16_t>(1) << static_cast<uint16_t>(code_size);
	mEndCode = mClearCode + 1;
	mHiCode = mClearCode + 1;
	mLast = DECODER_INVALID;
	mWidth = 1 + mCodeSize;
	mNBits = 0;
	mBits = 0;
	mOverflow = static_cast<uint16_t>(1) << mWidth;
	mO = 0;
	mSuffix.resize(1<<MAX_WIDTH);
	mPrefix.resize(1<<MAX_WIDTH);
	mOutput.resize(2 * (1<<MAX_WIDTH));
}

bool LzwReader::decode(CIter begin, CIter end) {
	mO = 0;
size_t	code_count = 0;
	while (begin != end) {
		// get next code
		bool			error = true;
		uint16_t		code = readCodeLsb(begin, end, error);
++code_count;
		if (error) {
			flush();
			return false;
		}

		// handle literal
		if (code < mClearCode) {
			mOutput[mO++] = static_cast<uint8_t>(code);
			if (mLast != DECODER_INVALID) {
				// Save what the hi code expands to.
				mSuffix[mHiCode] = static_cast<uint8_t>(code);
				mPrefix[mHiCode] = mLast;
			}

		// handle clear
		} else if (code == mClearCode) {
			mWidth = 1 + mCodeSize;
			mHiCode = mEndCode;
			mOverflow = 1 << mWidth;
			mLast = DECODER_INVALID;
			continue;

		// handle end
		} else if (code == mEndCode) {
			flush();
			return true;

		} else if (code <= mHiCode) {
			uint16_t c = code;
			size_t	i = mOutput.size()-1;
			if (code == mHiCode) {
				// code == hi is a special case which expands to the last expansion
				// followed by the head of the last expansion. To find the head, we walk
				// the prefix chain until we find a literal code.
				c = mLast;
				while (c >= mClearCode) {
					c = mPrefix[c];
				}
				mOutput[i] = static_cast<uint8_t>(c);
				i--;
				c = mLast;
			}
			// Copy the suffix chain into output and then write that to w.
			while (c >= mClearCode) {
				mOutput[i] = mSuffix[c];
				i--;
				c = mPrefix[c];
			}
			mOutput[i] = static_cast<uint8_t>(c);
			for (size_t k=i; k<mOutput.size(); ++k) {
				mOutput[mO++] = mOutput[k];
			}
			if (mLast != DECODER_INVALID) {
				// Save what the hi code expands to.
				mSuffix[mHiCode] = static_cast<uint8_t>(c);
				mPrefix[mHiCode] = mLast;
			}

		// handle error
		} else {
			flush();
			throw std::runtime_error("LZW decompressor on invalid code");
			return false;
		}

		mLast = code;
		mHiCode = mHiCode + 1;
		if (mHiCode >= mOverflow) {
			if (mWidth == MAX_WIDTH) {
				mLast = DECODER_INVALID;
			} else {
				++mWidth;
				mOverflow <<= 1;
			}
		}
		if (mO >= FLUSH_BUFFER) {
			flush();
		}
	}

	flush();
	return true;
}

uint16_t LzwReader::readCodeLsb(CIter &begin, CIter end, bool &error) {
	error = true;

	while (mNBits < mWidth) {
		if (begin == end) return 0;
		const uint8_t	b = reinterpret_cast<const unsigned char&>(*begin);
		++begin;

		mBits |= static_cast<uint32_t>(b) << mNBits;
		mNBits += 8;
	}

	uint16_t			code = static_cast<uint16_t>(mBits & ((1<<mWidth) - 1));
	mBits >>= mWidth;
	mNBits -= mWidth;
	error = false;
	return code;
}

void LzwReader::flush() {
	if (mFlushFn && mO > 0) {
		mAnswer.clear();
		mAnswer.reserve(mO);
		mAnswer.insert(mAnswer.begin(), mOutput.begin(), mOutput.begin()+mO);
		mFlushFn(mAnswer);
	}
	mO = 0;
}

} // namespace gif
