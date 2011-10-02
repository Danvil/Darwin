/*
 * Scheme.cpp
 *
 *  Created on: 29.11.2009
 *      Author: david
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Scheme.h"
#include <FTGL/ftgl.h>
#include <GL/glew.h>
#include <iostream>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

PTR(Scheme) Scheme::s_default;

void Scheme::LoadDefault(const std::string& font_path, bool use_light)
{
	s_default.reset(new Scheme(font_path + "/font-bold", font_path + "/font-normal", use_light));
}

void Scheme::Load(const std::string& normal_font_fn, const std::string& bold_font_fn, bool use_light)
{
	s_default.reset(new Scheme(normal_font_fn, bold_font_fn, use_light));
}

// TODO this is currently duplicated in Painter::print
//#define FTGL_FONT_STYLE FTGLPixmapFont
#define FTGL_FONT_STYLE FTTextureFont

Scheme::Scheme(const std::string& normal_font_fn, const std::string& bold_font_fn, bool use_light)
{
	// title font
	fontTitle = new FTGL_FONT_STYLE(bold_font_fn.c_str());
	if(fontTitle->Error()) {
		std::cerr << "Font initialization error" << std::endl;
	}
	fontTitle->FaceSize(18);

	// text font
	fontText = new FTGL_FONT_STYLE(normal_font_fn.c_str());
	if(fontText->Error()) {
		std::cerr << "Font initialization error" << std::endl;
	}
	fontText->FaceSize(14);

//	fontTitleSize = new FTGL_FONT_STYLE(bold_font_fn.c_str());
//	if(fontTitleSize->Error()) {
//		std::cerr << "Font initialization error" << std::endl;
//	}
//	fontTitleSize->FaceSize(18);
	fontTitleSize = fontTitle;

//	fontTextSize = new FTGL_FONT_STYLE(normal_font_fn.c_str());
//	if(fontTextSize->Error()) {
//		std::cerr << "Font initialization error" << std::endl;
//	}
//	fontTextSize->FaceSize(14);
	fontTextSize = fontText;

	if(use_light) {
		setSchemeLight();
	}
	else {
		setSchemeDark();
	}
}

Scheme::~Scheme()
{
//	delete fontText;
//	delete fontTitle;
}

void Scheme::setSchemeDark()
{
	colorBackground = Colorf(0.10f, 0.15f, 0.30f, 0.97f);
	colorControl = Colorf(0.40f, 0.40f, 0.40f, 1.00f);
	colorControlLight = Colorf(0.90f, 0.90f, 0.90f, 1.00f);
	colorControlDark = Colorf(0.05f, 0.05f, 0.05f, 1.00f);
	colorHighlight = Colorf(0.90f, 0.80f, 0.20f, 1.00f);
	colorBorder = Colorf(0.10f, 0.10f, 0.10f, 0.97f);
	colorHeader = Colorf(0.10f, 0.10f, 0.10f, 0.90f);
	colorText = Colorf(1.00f, 1.00f, 1.00f, 1.00f);
	colorTitle = Colorf(1.00f, 1.00f, 1.00f, 1.00f);
}

void Scheme::setSchemeLight()
{
	colorBackground = Colorf(0.90f, 0.90f, 0.90f, 0.95f);
	colorControl = Colorf(0.60f, 0.60f, 0.60f, 1.00f);
	colorControlLight = Colorf(0.80f, 0.80f, 0.80f, 1.00f);
	colorControlDark = Colorf(0.20f, 0.20f, 0.20f, 1.00f);
	colorHighlight = Colorf(0.20f, 0.20f, 0.60f, 1.00f);
	colorBorder = Colorf(0.75f, 0.75f, 0.75f, 0.97f);
	colorHeader = Colorf(0.45f, 0.45f, 0.45f, 0.90f);
	colorText = Colorf(0.00f, 0.00f, 0.00f, 1.00f);
	colorTitle = Colorf(1.00f, 1.00f, 1.00f, 1.00f);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
