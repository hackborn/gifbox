#ifndef KT_VIEW_IMAGEVIEW_H_
#define KT_VIEW_IMAGEVIEW_H_

#include <cinder/gl/Texture.h>
#include <cinder/Surface.h>
#include <kt/view/view.h>

namespace kt {
namespace view {

/**
 * @class kt::view::Image
 * @brief Display an image.
 */
class Image : public kt::view::View {
public:
	// Pass-through convenience to v->addChildOrThrow
	static Image&				make(View &parent, kt::Cns &cns)
										{ return parent.addChildOrThrow<kt::view::Image>(new kt::view::Image(cns)); }
	static Image&				make(View &parent, kt::Cns &cns, const std::string &filename)
										{ Image& ans = parent.addChildOrThrow<kt::view::Image>(new kt::view::Image(cns)); ans.setFilename(filename); return ans; }
	Image() = delete;
	Image(const Image&) = delete;
	Image(kt::Cns&);

	bool						empty() const;

	void						setFilename(const std::string &fn);
	void						setSurface(const ci::Surface8u&);

protected:
	void						onDraw(const kt::view::DrawParams&) override;

private:
	void						assignTexture(const std::function<ci::gl::TextureRef(void)>&);

	using base = kt::view::View;
	ci::gl::TextureRef			mTexture;
};

} // namespace view
} // namespace kt

#endif
