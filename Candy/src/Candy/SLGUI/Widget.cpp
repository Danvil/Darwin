/*
 * Widget.cpp
 *
 *  Created on: 29.11.2009
 *      Author: david
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Widget.h"
#include "Scheme.h"
#include "Painter.h"
#include <GL/glew.h>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Widget::Widget() {
	pos = Point2(0, 0);
	size = Point2(200, 150);
	opacity = 1.00;
}

Widget::~Widget() {
}

bool Widget::hits(const Point2& p_global) {
	if(!isVisible) {
		return false;
	}
	Point2 p = p_global - getWorldPosition();
	return (0 <= p.x() && p.x() <= size.x() && 0 <= p.y() && p.y() <= size.y());
}

Colorf Widget::getBackgroundColor() const {
	Colorf c = Scheme::Default()->colorBackground;
	c.w() = opacity;
	return c;
}

Colorf Widget::getTextColor() const {
	Colorf c = Scheme::Default()->colorText;
	c.w() = opacity;
	return c;
}

Colorf Widget::getHighlightColor() const {
	Colorf c = Scheme::Default()->colorHighlight;
	c.w() = opacity;
	return c;
}

void Widget::render() {
	if(opacity < 1.0f) {
		// enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	// background
	Painter::Singleton->setColor(getBackgroundColor());
	Painter::Singleton->paintQuad(size);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
