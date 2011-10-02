/*
 * Manager.cpp
 *
 *  Created on: 29.11.2009
 *      Author: david
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Manager.h"
#include "Painter.h"
#include "Scheme.h"
#include "Widgets/Window.h"
//#include <Danvil/SimpleEngine/Material.h>
#include <boost/foreach.hpp>
#include <GL/glew.h>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Manager::Manager()
{
	didAteEvent = false;
	SLGUI::Painter::Singleton->scheme = SLGUI::Scheme::Default();
	viewport_width_ = 800;
	viewport_height_ = 600;
}

Manager::~Manager() {
}

void Manager::add(const PTR(Widget)& a) {
	windows.insert(a);
}

void Manager::remove(const PTR(Widget)& a) {
	windows.erase(a);
}

void Manager::SetViewPortSize(unsigned int w, unsigned int h)
{
	viewport_width_ = w;
	viewport_height_ = h;
}

PTR(Widget) Manager::pick(const Point2& p) {
	// find widget with the intersection
	// top windows are first in the list!
	BOOST_FOREACH(const PTR(Widget)& w, windows) {
		PTR(Widget) r = pick(w, p);
		if(r) {
			return r;
		}
	}
	// did not have an intersection with any gui element
	return PTR(Widget)();
}

PTR(Widget) Manager::pick(const PTR(Object)& o, const Point2& p) {
	PTR(Widget) result;
	// check if we have a widget here (only widgets can be selected)
	if(o->isWidget()) {
		PTR(Widget) w = o->toWidget();
		// only proceed here when we hit the widget
		// sub widgets are clipped by the parent widget!
		if(!w->hits(p)) {
			return result;
		}
		// if none of the children is hit later the parent is hit
		result = w;
	}
	// look for intersection under children
	// top widget is first in the list!
	BOOST_FOREACH(const PTR(Object)& child, o->children) {
		PTR(Widget) r = pick(child, p);
		if(r) {
			// found a child which is picked!
			return r;
		}
	}
	// return either parent (if it is a widget) or none
	return result;
}

void Manager::render() {
	// set gui viewport
	unsigned int w = viewport_width_;//viewport->getWidth();
	unsigned int h = viewport_height_;//viewport->getHeight();
	glUseProgram(0);
	glDisable(GL_TEXTURE_1D);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_3D);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1, +1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// disable depth buffer so that everything is rendered
	glDepthMask(0);
	Painter::Singleton->viewportSize.x() = float(w);
	Painter::Singleton->viewportSize.y() = float(h);
//	glEnable(GL_SCISSOR_TEST);
	// render gui
//	Danvil::SimpleEngine::Material::UseStandard();
	BOOST_FOREACH(const PTR(Widget)& w, windows) {
//		glScissor(w->pos.x(), w->pos.y(), w->size.x(), w->size.y()+23);
		render(w);
	}
	// re-enable depth buffer
//	glDisable(GL_SCISSOR_TEST);
	glDepthMask(1);
	// restore scene viewport
//	viewport->apply();
}

void Manager::render(const PTR(Object)& o) {
	// check if this is a widget and render it if so
	PTR(Widget) w;
	if(o->isWidget()) {
		w = o->toWidget();
		if(!w->isVisible) {
			return;
		}
	}
	// apply transformation before rendering
	glPushMatrix();
	glTranslatef(o->pos.x(), -o->pos.y(), 0.0f);
	// render self
	if(w) {
		w->render();
	}
	// render last child first
	// top widget is first in the list!
	BOOST_REVERSE_FOREACH(const PTR(Object)& child, o->children) {
		render(child);
	}
	glPopMatrix();
}

void Manager::onMousePressed(KeyboardModifiers mod, MouseButton button, int x, int y) {
	Point2 p(x, y);
	PTR(Widget) w = pick(p);
	if(w) {
		w->onMouseDown(p - w->getWorldPosition());
		// only move windows
		if(DanvilIsOfType<Window,Widget>(w)) {
			heldWidget = w;
			heldStart = p;
		}
		didAteEvent = true;
	} else {
		didAteEvent = false;
	}
}

void Manager::onMouseReleased(KeyboardModifiers mod, MouseButton button, int x, int y) {
	Point2 p(x, y);
	PTR(Widget) w = pick(p);
	if(w) {
		w->onMouseUp(p - w->getWorldPosition());
		didAteEvent = true;
	} else {
		didAteEvent = false;
	}
	heldWidget.reset();
}

void Manager::onMouseMove(KeyboardModifiers mod, MouseButton buttons, int x, int y) {
	Point2 p(x, y);
	PTR(Widget) w = pick(p);
	// process mouse over
	if(w != lastWidget) {
		if(lastWidget) {
			lastWidget->onMouseLeave();
		}
		lastWidget = w;
		if(lastWidget) {
			lastWidget->onMouseEnter();
		}
	}
	// move windows
	if(heldWidget) {
		heldWidget->pos += p - heldStart;
		heldStart = p;
	}
	 // if mouse is over widget we ate the event
	didAteEvent = w;
}

void Manager::onWheel(KeyboardModifiers mod, int delta) {
	didAteEvent = false;
}

void Manager::onKeyPressed(KeyboardModifiers mod, int key) {
	if(lastWidget) {
		lastWidget->onKeyPress(key);
	}
	didAteEvent = false;
}

void Manager::onKeyReleased(KeyboardModifiers mod, int key) {
	didAteEvent = false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
