#ifndef APP_CINDERGIF_TEXTUREGIFLIST_H_
#define APP_CINDERGIF_TEXTUREGIFLIST_H_

#include <cinder/gl/Texture.h>
#include <gifwrap/gif_list.h>

namespace cs {

/**
 * @class cs::TextureGifList
 * @brief Provide a list of local Textures and convert between gif bitmaps
 * and the local format.
 */
class TextureGifList : public gif::List<ci::gl::TextureRef> {
public:
	TextureGifList();

	void				readerFinished() override;

private:
	ci::gl::TextureRef	convert(const gif::Bitmap&);

	using base = gif::List<ci::gl::TextureRef>;
	ci::Surface8u		mSurface;
};

} // namespace cs

#endif
