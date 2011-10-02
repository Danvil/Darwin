/*
 * Frame.cpp
 *
 *  Created on: 29.11.2009
 *      Author: david
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Frame.h"
#include "../Scheme.h"
#include "../Painter.h"
#include <GL/glew.h>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Frame::Frame() {
	borderWidth = 1;
}

Frame::~Frame() {
}

void Frame::render() {
	Widget::render();
	if(borderWidth > 0) {
		Painter::Singleton->setColor(Scheme::Default()->colorBorder);
		Painter::Singleton->paintQuadBorder(size, borderWidth);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
