#ifndef GIFWRAP_GIFFILE_H_
#define GIFWRAP_GIFFILE_H_

#include <stdexcept>
#include "gif_algorithm.h"
#include "gif_block.h"
#include "gif_list.h"
#include "lzw_writer.h"

namespace gif {

// Decide how to create color table(s).
// * kGlobalTableFromFirst -- create a global color table based solely on
// the first frame of data. This is (potentially) the most memory-efficient
// mode, as each frame can be written and then discarded.
// * kGlobalTableFromAll -- create a global color table based on all frames.
// This will likely result in the best balance of final output quality and
// file size, but at an up-front memory cost of requiring to load all frames
// of data.
// * kLocalTable -- a local color table is created for each frame.
enum class TableMode {	kGlobalTableFromFirst,
						kGlobalTableFromAll,
						kLocalTable };

/**
 * @class gif::Reader
 * @brief Load a GIF file into a sequence of images.
 * GIF spec: http://www.w3.org/Graphics/GIF/spec-gif89a.txt
 */
class Reader {
public:
	Reader(std::string path);

	// Given a file path, load all frames of data to output.
	// This peforms no validation that the file is valid.
	// Answer false on error.
	bool				read(gif::ListConstructor &output);

private:
	std::string			mPath;
};

/**
 * @class gif::WriterSettings
 * @brief Private internal class.
 */
class WriterSettings {
public:
	WriterSettings() { }

	void						makeGlobalTable(const gif::Bitmap&);

	int32_t						mWidth = 0,
								mHeight = 0;
	uint8_t						mBackgroundColorIndex = 0;
	TableMode					mTableMode = TableMode::kGlobalTableFromFirst;
	gif::Palette				mGlobalPalette;

	BitmapToPaletteRef			mBitmapToPalette;
	ToColorIndexRef				mToColorIndex;
	ToPalettedBitmapRef			mToPalettedBitmap;
};

/**
 * @class gif::WriterT
 * @brief Write image frames into a GIF file.
 * GIF spec: http://www.w3.org/Graphics/GIF/spec-gif89a.txt
 */
template <typename T>
class WriterT {
public:
	WriterT(std::function<void(const T&, gif::Bitmap&)>, std::string path);
	virtual ~WriterT();

	WriterT&				setTableMode(TableMode m) { mSettings.mTableMode = m; return *this; }
	WriterT&				setBackgroundColorIndex(const uint8_t v) { mSettings.mBackgroundColorIndex = v; return *this; }

	// Add the frame to the file. Throw on error.
	void					writeFrame(const T&);

	// Various pluggable algorithms. Ignore for defaults.

	// Create a palette from a bitmap.
	WriterT&				setBitmapToPalette(BitmapToPaletteRef a = nullptr) { mSettings.mBitmapToPalette = a; return *this; }
	// When converting a bitmap into a paletted bitmap, this determines how to match the RGBA color
	// to the nearest palette color.
	WriterT&				setToColorIndex(ToColorIndexRef a = nullptr) { mSettings.mToColorIndex = a; return *this; }
	// When converting a bitmap to a paletted bitmap, this determines how the full bitmap is processed.
	WriterT&				setToPalettedBitmap(ToPalettedBitmapRef a = nullptr) { mSettings.mToPalettedBitmap = a; return *this; }

private:
	WriterSettings			mSettings;
	std::function<void(const T&, gif::Bitmap&)>
							mConvertFn;
	std::string				mPath;
	bool					mNeedsHeader = true;
	TableMode				mTableMode = TableMode::kGlobalTableFromFirst;
	gif::Bitmap				mPixels;
	PalettedBitmap			mPalettedBitmap;
	// Store the encoder so I can reuse memory
	LzwWriter				mLzwWriter;
	std::ofstream			mStream;
	WriterBuffer			mBlockBuffer;
};

/**
 * @class gif::Writer
 * @brief A writer that specializes on the GIF bitmap storage class,
 * if you want to handle all the pixel wrangling yourself.
 */
class Writer : public WriterT<gif::Bitmap> {
public:
	Writer(std::string path);

private:
	void					convert(const gif::Bitmap &src, gif::Bitmap &dst) const;

	using base = WriterT<gif::Bitmap>;
};

/**
 * @class gif::WriterT IMPLEMENTATION
 */
template <typename T>
WriterT<T>::WriterT(std::function<void(const T&, gif::Bitmap&)> convert_fn, std::string path)
		: mConvertFn(convert_fn)
		, mPath(path)
		, mBlockBuffer(mStream) {
}

template <typename T>
WriterT<T>::~WriterT() {
	if (mStream) {
		// Ending trailer byte
		mStream << 0x3b;
	}
}

template <typename T>
void WriterT<T>::writeFrame(const T &t) {
	if (!mConvertFn) throw std::runtime_error("gif::Writer<T>::writeFrame() has no convert function");
	mConvertFn(t, mPixels);
	if (mPixels.empty()) throw std::runtime_error("gif::Writer<T>::writeFrame() conversion failed");

	// Initialize my algorithms
	if (!mSettings.mBitmapToPalette) mSettings.mBitmapToPalette = BitmapToPalette::create();
	if (!mSettings.mToColorIndex) mSettings.mToColorIndex = ToColorIndex::create();
	if (!mSettings.mToPalettedBitmap) mSettings.mToPalettedBitmap = ToPalettedBitmap::create();

	// Delay the initial writing until I receive frame data as a convenience, so clients don't
	// need to specify a screen size but instead it can just be pulled from the bitmap.
	if (mNeedsHeader) {
		mNeedsHeader = false;

		mStream = std::ofstream(mPath, std::ios::out | std::ios::binary);

		mSettings.mWidth = mPixels.mWidth;
		mSettings.mHeight = mPixels.mHeight;
		if (mSettings.mWidth >= 1<<16 || mSettings.mHeight >= 1<<16) throw std::runtime_error("gif::Writer<T>::writeFrame() image is too large");
		if (mSettings.mTableMode == TableMode::kGlobalTableFromFirst) {
			mSettings.makeGlobalTable(mPixels);
		}
		write_header(mSettings, mStream);
	}

	// Write the image data
	mSettings.mToColorIndex->setTo(mSettings.mGlobalPalette);
	mSettings.mToPalettedBitmap->convert(mPixels, mSettings.mToColorIndex, mPalettedBitmap);
	if (mPalettedBitmap.empty()) throw std::runtime_error("gif::Writer<T>::writeFrame() failed to convert to paletted bitmap");
	write_table_based_image(mSettings, mPixels, mPalettedBitmap, mLzwWriter, mBlockBuffer, mStream);
}

// Private writing API
void		write_header(const gif::WriterSettings&, std::ostream &output);
void		write_table_based_image(const gif::WriterSettings&, const gif::Bitmap&, const PalettedBitmap&,
									LzwWriter&, WriterBuffer&, std::ostream &output);

} // namespace gif

#endif
