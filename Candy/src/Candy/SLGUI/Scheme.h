/*
 * Scheme.h
 *
 *  Created on: 29.11.2009
 *      Author: david
 */

#ifndef STARLORD_SLGUI_SCHEME_H_
#define STARLORD_SLGUI_SCHEME_H_
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Common.h"
#include <string>
class FTFont;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class CANDY_API Scheme
{
public:
	virtual ~Scheme();

	static void LoadDefault(const std::string& font_path, bool use_light=true);

	static void Load(const std::string& normal_font_fn, const std::string& bold_font_fn, bool use_light=true);

	static PTR(Scheme) Default() {
		return s_default;
	}

	Colorf colorBackground;
	Colorf colorControl;
	Colorf colorControlLight;
	Colorf colorControlDark;
	Colorf colorHighlight;
	Colorf colorBorder;
	Colorf colorHeader;
	Colorf colorText;
	Colorf colorTitle;

private:
	Scheme(const std::string& normal_font_fn, const std::string& bold_font_fn, bool use_light);

	void setSchemeDark();

	void setSchemeLight();

private:
	static PTR(Scheme) s_default;

	FTFont* fontText;
	FTFont* fontTitle;

	// hack for BBox calls which crashs font rendering ...
	FTFont* fontTextSize;
	FTFont* fontTitleSize;

	friend class Painter;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif
