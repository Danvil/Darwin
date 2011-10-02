/*
 * Painter.h
 *
 *  Created on: 06.12.2009
 *      Author: david
 */

#ifndef STARLORD_SLGUI_PAINTER_H_
#define STARLORD_SLGUI_PAINTER_H_
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Common.h"
class FTFont;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class Scheme;

class CANDY_API Painter
{
public:
	enum ETextStyle { Normal, Strong };

public:
	Painter();

	virtual ~Painter();

	void paintQuad(const Point2& size);

	void paintQuadWithTexture(const Point2& size);

	void paintQuadWithTexture(const Point2& size, float u, float v);

	void paintQuad(const Point2& pos, const Point2& size);

	void paintQuadWithTexture(const Point2& pos, const Point2& size);

	void paintQuadWithTexture(const Point2& pos, const Point2& size, float u, float v);

	void paintQuadBorder(const Point2& size, unsigned int thickness=1);

	void paintQuadBorder(const Point2& pos, const Point2& size, unsigned int thickness=1);

	void paintLine(const Point2& a, const Point2& b);

	void processVertex(float x, float y);

	void processVertex(const Point2& a);

	void setColor(const Colorf& c);

	void print(ETextStyle style, const std::string& s, float x, float y, const Colorf& c);

	void size(ETextStyle style, const std::string& s, float& w, float& h);

	Point2 size(ETextStyle style, const std::string& s);

public:
	Point2 viewportSize;

	PTR(Scheme) scheme;

private:
	FTFont* getFont(ETextStyle style) const;
	FTFont* getFontSize(ETextStyle style) const;

public:
	static PTR(Painter) Singleton;

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif
