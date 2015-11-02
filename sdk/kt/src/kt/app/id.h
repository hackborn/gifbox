#ifndef KT_APP_ID_H_
#define KT_APP_ID_H_

#include <cstdint>

namespace kt {
// edit_id is intended as a simple "modified" counter so clients can identify
// when a change was made to an item.
using edit_id = std::int32_t;

/**
 * @class kt::Version
 * @brief A standard version number (major.minor.bug, i.e. 1.0.0) reduced to
 * a 32-bit int (10 bits major, 10 bits minor, 11 bits bug, 1 bit to signify internal or release).
 */
class Version {
public:
	Version(const uint32_t raw_version) : mVersion(raw_version) { }
	Version(const uint32_t major, const uint32_t minor, const uint32_t bug) { set(major, minor, bug); }

	bool					operator==(const Version &o) const { return mVersion == o.mVersion; }
	bool					operator!=(const Version &o) const { return mVersion != o.mVersion; }

	void					set(const uint32_t major, const uint32_t minor, const uint32_t bug) {
		mVersion = ((major&BITS_10) << 21) | ((minor&BITS_10) << 11) | (bug&BITS_11);
	}

	void					get(uint32_t &major, uint32_t &minor, uint32_t &bug) const {
		major = (mVersion>>21)&BITS_10;
		minor = (mVersion>>11)&BITS_10;
		bug = mVersion&BITS_11;
	}

	uint32_t				getRaw() const { return mVersion; }

	// A version can be flagged as internal, to separate daily builds from a release.
	void					setInternal(const bool b) {
		(b ? (mVersion |= BITS_INTERNAL) : (mVersion &= ~BITS_INTERNAL));
	}

	bool					isInternal() const {
		return (mVersion&BITS_INTERNAL) != 0;
	}

private:
	static const uint32_t	BITS_10 = 0x3ff;
	static const uint32_t	BITS_11 = 0x7ff;
	static const uint32_t	BITS_INTERNAL = 0x80000000;
	uint32_t				mVersion = 0;
};

} // namespace kt

#endif
