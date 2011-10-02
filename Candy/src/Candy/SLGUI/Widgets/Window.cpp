/*
 * Window.cpp
 *
 *  Created on: 28.12.2009
 *      Author: david
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Window.h"
#include "../Scheme.h"
#include "../Painter.h"
#include <GL/glew.h>
#include <boost/bind.hpp>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const float HEADER_HEIGHT = 22;

Window::Window()
:	windowTitle("SLGUI") {
	notifierConnection = onAddChild.connect(boost::bind(&Window::childAdded, this, _1));
}

Window::~Window() {
	notifierConnection.disconnect();
}

void Window::render() {
	// render body
	Widget::render();
	// header
	Painter::Singleton->setColor(Scheme::Default()->colorHeader);
	Painter::Singleton->paintQuad(Point2(0, 0), Point2(size.x(), HEADER_HEIGHT));
	// header title
	Painter::Singleton->print(Painter::Strong, windowTitle.c_str(), 4, 3, Scheme::Default()->colorTitle);
}

void Window::childAdded(const PTR(Object)& a) {
	// assure that child widgets do not overlap with the title bar
	if(a->getPosition().y() < HEADER_HEIGHT) {
		a->setPosition(Point2(a->getPosition().x(), HEADER_HEIGHT));
	}
}

PTR(Window) Window::Factor(const std::string& title) {
	PTR(Window) w(new Window());
	w->windowTitle = title;
	return w;
}

PTR(Window) Window::Factor(const std::string& title, float px, float py) {
	PTR(Window) w = Factor(title);
	w->pos.x() = px;
	w->pos.y() = py;
	return w;
}

PTR(Window) Window::Factor(const std::string& title, float px, float py, float sx, float sy) {
	assert(sx > 0);
	assert(sy > 0);
	PTR(Window) w = Factor(title, px, py);
	w->size.x() = sx;
	w->size.y() = sy;
	return w;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
