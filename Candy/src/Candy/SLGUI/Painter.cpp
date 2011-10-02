/*
 * Painter.cpp
 *
 *  Created on: 06.12.2009
 *      Author: david
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Painter.h"
#include "Scheme.h"
//#include <Danvil/SimpleEngine/Auxiliary/GlHelpers.h>
#include <FTGL/ftgl.h>
#include <GL/glew.h>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Painter::Painter() {
	viewportSize = Point2(300, 300);
}

Painter::~Painter() {

}

void Painter::paintQuad(const Point2& size) {
	paintQuad(Point2(0,0), size);
}

void Painter::paintQuadWithTexture(const Point2& size) {
	paintQuadWithTexture(Point2(0,0), size);
}

void Painter::paintQuadWithTexture(const Point2& size, float u, float v) {
	paintQuadWithTexture(Point2(0,0), size, u, v);
}

void Painter::paintQuad(const Point2& pos, const Point2& size) {
	float px = pos.x();
	float py = pos.y();
	float sx = size.x();
	float sy = size.y();
	glBegin(GL_QUADS);
	processVertex(px, py);
	processVertex(px, py + sy);
	processVertex(px + sx, py + sy);
	processVertex(px + sx, py);
	glEnd();
}

void Painter::paintQuadWithTexture(const Point2& pos, const Point2& size) {
	float px = pos.x();
	float py = pos.y();
	float sx = size.x();
	float sy = size.y();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	processVertex(px, py);
	glTexCoord2f(0.0f, 0.0f);
	processVertex(px, py + sy);
	glTexCoord2f(1.0f, 0.0f);
	processVertex(px + sx, py + sy);
	glTexCoord2f(1.0f, 1.0f);
	processVertex(px + sx, py);
	glEnd();
}

void Painter::paintQuadWithTexture(const Point2& pos, const Point2& size, float u, float v) {
	float px = pos.x();
	float py = pos.y();
	float sx = size.x();
	float sy = size.y();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, v);
	processVertex(px, py);
	glTexCoord2f(0.0f, 0.0f);
	processVertex(px, py + sy);
	glTexCoord2f(u, 0.0f);
	processVertex(px + sx, py + sy);
	glTexCoord2f(u, v);
	processVertex(px + sx, py);
	glEnd();
}

void Painter::paintQuadBorder(const Point2& size, unsigned int thickness) {
	paintQuadBorder(Point2(0,0), size, thickness);
}

void Painter::paintQuadBorder(const Point2& pos, const Point2& size, unsigned int thickness) {
	float px = pos.x();
	float py = pos.y();
	float sx = size.x();
	float sy = size.y();
	glLineWidth(float(thickness));
	glBegin(GL_LINE_STRIP);
	processVertex(px, py);
	processVertex(px, py + sy);
	processVertex(px + sx, py + sy);
	processVertex(px + sx, py);
	processVertex(px, py);
	glEnd();
}

void Painter::paintLine(const Point2& a, const Point2& b) {
	glBegin(GL_LINES);
	processVertex(a);
	processVertex(b);
	glEnd();
}

void Painter::processVertex(float x, float y) {
	glVertex2f(x, viewportSize.y() - y);
}

void Painter::processVertex(const Point2& a) {
	glVertex2f(a.x(), viewportSize.y() - a.y());
}

void Painter::setColor(const Colorf& color) {
	glColor4f(color.x(), color.y(), color.z(), color.w());
}

FTFont* Painter::getFont(ETextStyle style) const {
	switch(style) {
	case Normal: return scheme->fontText;
	case Strong: return scheme->fontTitle;
	default: assert(false); return 0;
	}
}

FTFont* Painter::getFontSize(ETextStyle style) const {
	switch(style) {
	case Normal: return scheme->fontTextSize;
	case Strong: return scheme->fontTitleSize;
	default: assert(false); return 0;
	}
}

void Painter::print(ETextStyle style, const std::string& s, float x, float y, const Colorf& color) {
	FTFont* font = getFont(style);
	// font positioning
	const float BASELINE_OFFSET = 5.0f;
	float w, h;
	size(style, s, w, h);
	// font render style
	enum EFontStyle { Pixmap, Texture };
	const EFontStyle FONT_STYLE = Texture; // TODO currently duplicated in Scheme.cpp
	switch(FONT_STYLE) {
	case Pixmap: {
		// we need to read the current position from OpenGL because
		// ftgl Render is absolute when using pixmaps
		float modelviewMatrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, modelviewMatrix);
		float base_x = modelviewMatrix[12];
		float base_y = modelviewMatrix[13];
		// need some trick to render colored code with pixmap fonts
		glPixelTransferf(GL_RED_BIAS, color.x() - 1.0f);
		glPixelTransferf(GL_GREEN_BIAS, color.y() - 1.0f);
		glPixelTransferf(GL_BLUE_BIAS, color.z() - 1.0f);
		// because our root is top, and font rendering is from down to up, we need to shift the
		// font render position down by the font height.
		float x_final = base_x + x;
		float y_final = viewportSize.y() + base_y - y - h + BASELINE_OFFSET;
		// do the rendering
		font->Render(s.c_str(), -1, FTPoint(x_final, y_final));
	} break;
	case Texture: {
		glColor4f(color.x(), color.y(), color.z(), color.w());
		float x_final = x;
		float y_final = viewportSize.y() - y - h + BASELINE_OFFSET;
		font->Render(s.c_str(), -1, FTPoint(x_final, y_final));
	} break;
	}
}

void Painter::size(ETextStyle style, const std::string& s, float& w, float& h) {
	FTFont* font = getFontSize(style);
	FTBBox box = font->BBox(s.c_str());
	w = float(box.Upper().X()) - float(box.Lower().X());
	//h = box.Upper().Y() - box.Lower().Y();
	// compute maximal font height
	box = font->BBox("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
	h = float(box.Upper().Y() - box.Lower().Y());
	// TODO fix this font position issue
}

Point2 Painter::size(ETextStyle style, const std::string& s) {
	float w, h;
	size(style, s, w, h);
	return Point2(w,h);
}

PTR(Painter) Painter::Singleton = PTR(Painter)(new Painter());

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
