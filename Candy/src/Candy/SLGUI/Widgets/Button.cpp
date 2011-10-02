/*
 * Button.cpp
 *
 *  Created on: 06.12.2009
 *      Author: david
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Button.h"
#include "../Scheme.h"
#include "../Painter.h"
#include <GL/glew.h>
#include <boost/bind.hpp>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Button::Button()
:	isPressed(false) {
	setText("test");
	borderWidth = 0;
}

Button::Button(const std::string& _text)
:	isPressed(false) {
	setText(_text);
	borderWidth = 0;
	c1 = onMouseDown.connect(boost::bind(&Button::setPressed, this, true));
	c2 = onMouseUp.connect(boost::bind(&Button::setPressed, this, false));
	c3 = onMouseLeave.connect(boost::bind(&Button::setPressed, this, false));
}

Button::~Button() {
	c1.disconnect();
	c2.disconnect();
	c3.disconnect();
}

void Button::setText(const std::string& _text) {
	text = _text;
	Painter::Singleton->size(Painter::Normal, text, size.x(), size.y());
	size.x() += 8;
	size.y() += 7;
}

void Button::render() {
	Colorf colorLU, colorRB;
	if(isPressed) {
		colorLU = Scheme::Default()->colorControlDark;
		colorRB = Scheme::Default()->colorControlLight;
	} else {
		colorLU = Scheme::Default()->colorControlLight;
		colorRB = Scheme::Default()->colorControlDark;
	}
	glDisable(GL_BLEND);
	Painter::Singleton->setColor(colorRB);
	Painter::Singleton->paintQuadBorder(Point2(1,1), size, 1);
	Painter::Singleton->setColor(colorLU);
	Painter::Singleton->paintQuadBorder(size, 1);
	Painter::Singleton->setColor(Scheme::Default()->colorControl);
	Painter::Singleton->paintQuad(Point2(0.5,0.5), size);

	Point2 d = 0.5f*(size - Painter::Singleton->size(Painter::Normal, text));
	Painter::Singleton->print(Painter::Normal, text, d.x(), d.y(), Scheme::Default()->colorText);
	glEnable(GL_BLEND);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
