#ifndef KT_VIEW_LABELVIEW_H_
#define KT_VIEW_LABELVIEW_H_

#include <kt/service/font_service.h>
#include <kt/view/view.h>

namespace kt {
namespace view {

/**
 * @class kt::view::Label
 * @brief Display a text label.
 */
class Label : public kt::view::View {
public:
	// Pass-through convenience to v->addChildOrThrow
	static Label&				make(View &parent, kt::Cns &cns)
										{ return parent.addChildOrThrow<kt::view::Label>(new kt::view::Label(cns)); }
	static Label&				make(View &parent, kt::Cns &cns, const std::string &font)
										{ Label& ans = parent.addChildOrThrow<kt::view::Label>(new kt::view::Label(cns)); ans.setFont(font); return ans; }
	Label(kt::Cns&);

	// Currently can only set to existing fonts that were installed on construction.
	void						setFont(const std::string &key);

	bool						empty() const { return mText.empty(); }
	void						setText(const std::string&);

protected:
	void						onDraw(const kt::view::DrawParams&) override;

private:
	void						measureSize();
	bool						isInvertedY();

	using base = kt::view::View;
	std::string					mText;
	const kt::FontService&		mFontService;
	ci::gl::TextureFontRef		mFont;
	float						mAscent,
								mDescent;
};

} // namespace view
} // namespace kt

#endif
