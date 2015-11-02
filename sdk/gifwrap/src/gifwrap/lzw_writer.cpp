#include "lzw_writer.h"

#include <iostream>
#include <unordered_map>

namespace gif {

namespace {
const uint32_t		MAX_CODE = (1<<12) - 1;
const uint32_t		INVALID_CODE = static_cast<uint32_t>((static_cast<uint64_t>(1)<<32)-1);
// There are 1<<12 possible codes, which is an upper bound on the number of
// valid hash table entries at any given point in time. tableSize is 4x that.
const uint32_t		TABLE_SIZE = 4 * (1<<12);
const uint32_t		TABLE_MASK = TABLE_SIZE - 1;
const uint32_t		INVALID_ENTRY = 0;

inline uint32_t		safeTableValue(const uint32_t key, const std::unordered_map<uint32_t, uint32_t> &table) {
	auto			found = table.find(key);
	if (found == table.end()) return INVALID_ENTRY;
	return found->second;
}

}

/**
 * @class gif::LzwWriter
 */
void LzwWriter::begin(	const uint8_t code_size,
						const std::function<void(const std::vector<uint8_t>&)> &flush_fn) {
	mFlushFn = flush_fn;
	mWidth = 1 + static_cast<uint32_t>(code_size);
	mCodeSize = code_size;
	mHi = (1<<mCodeSize) + 1;
	mOverflow = 1<<(mCodeSize+1);
	mSavedCode = INVALID_CODE;
	mTable.clear();

	// Write initial clear code
	writeCodeLsb(clearCode());
}

void LzwWriter::encode(const std::vector<uint8_t> &bm) {
	if (bm.empty()) return;

	uint32_t			code = mSavedCode;
	auto				bm_it = bm.begin(), end_it = bm.end();
	if (code == INVALID_CODE) {
		// The first code is always a literal code
		code = *bm_it;
		++bm_it;
	}
	while (bm_it != end_it) {
		uint32_t		literal = *bm_it;
		uint32_t		key = (code<<8) | literal;

		// If there is a hash table hit for this key then we continue the loop
		// and do not emit a code yet.
		uint32_t		hash = (key>>12 ^ key) & TABLE_MASK;
		{
			uint32_t	h = hash, t = safeTableValue(hash, mTable);
			while (t != INVALID_ENTRY) {
				if (key == t>>12) {
					code = t&MAX_CODE;
					goto endloop;
				}
				h = (h+1)&TABLE_MASK;
				t = safeTableValue(h, mTable);
			}
		}

		// Otherwise, write the current code, and literal becomes the start of
		// the next emitted code.
		writeCodeLsb(code);

		code = literal;
		// Increment e.hi, the next implied code. If we run out of codes, reset
		// the encoder state (including clearing the hash table) and continue.
		const IncError		ierr = incHi();
		if (ierr != IncError::kNone) {
			if (ierr == IncError::kOutOfCodes) {
				goto endloop;
			}
			return; // error
		}

		// Otherwise, insert key -> e.hi into the map that e.table represents.
		while (true) {
			if (safeTableValue(hash, mTable) == INVALID_ENTRY) {
				mTable[hash] = (key << 12) | mHi;
				break;
			}
			hash = (hash + 1) & TABLE_MASK;
		}

endloop:
		++bm_it;
	}
	mSavedCode = code;
	close();
}

void LzwWriter::close() {
	if (mSavedCode != INVALID_CODE) {
		writeCodeLsb(mSavedCode);
	}
	// Write the eof code.
	const uint32_t	eof = (static_cast<uint32_t>(1)<<mCodeSize) + 1;
	writeCodeLsb(eof);

	// Write the final bits.
	if (mNBits > 0 && mFlushFn) {
		mOutput.clear();
		mOutput.push_back(static_cast<uint8_t>(mBits));
		mFlushFn(mOutput);
	}
}

void LzwWriter::writeCodeLsb(const uint32_t code) {
	mBits |= code << mNBits;
	mNBits += mWidth;
	while (mNBits >= 8) {
		if (mFlushFn) {
			mOutput.clear();
			mOutput.push_back(static_cast<uint8_t>(mBits));
			mFlushFn(mOutput);
		}

		mBits >>= 8;
		mNBits -= 8;
	}
}

LzwWriter::IncError LzwWriter::incHi() {
	++mHi;
	if (mHi == mOverflow) {
		++mWidth;
		mOverflow <<= 1;
	}
	if (mHi == MAX_CODE) {
		const uint32_t		clear = clearCode();
		writeCodeLsb(clear);

		mWidth = mCodeSize + 1;
		mHi = clear + 1;
		mOverflow = clear << 1;
		for (auto& kv : mTable) kv.second = INVALID_ENTRY;
		return IncError::kOutOfCodes;
	}
	return IncError::kNone;
}

/**
 * @class gif::WriterBuffer
 */
void WriterBuffer::write(const std::vector<uint8_t> &data) {
	// Append to my current data
	if (!data.empty()) {
		mBuffer.reserve(mBuffer.size() + data.size());
		mBuffer.insert(mBuffer.end(), data.begin(), data.end());
	}
	writeFullBlocks();
}

void WriterBuffer::terminate() {
	writeFullBlocks();
	if (!mBuffer.empty()) {
		writeBlock(mBuffer.size());
	}
	mStream << static_cast<uint8_t>(0);
}

void WriterBuffer::writeFullBlocks() {
	while (mBuffer.size() >= mBlockSize) {
		writeBlock(mBlockSize);
	}
}

void WriterBuffer::writeBlock(const size_t size) {
	if (mBuffer.size() < size) return; // error

	mStream << static_cast<uint8_t>(size);
	mStream.write((const char*)mBuffer.data(), size);
	mBuffer.erase(mBuffer.begin(), mBuffer.begin()+size);
}

} // namespace gif
