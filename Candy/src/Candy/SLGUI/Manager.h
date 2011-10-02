/*
 * Manager.h
 *
 *  Created on: 29.11.2009
 *      Author: david
 */

#ifndef STARLORD_SLGUI_MANAGER_H_
#define STARLORD_SLGUI_MANAGER_H_
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Widget.h"
//#include <Danvil/SimpleEngine/IRenderable.h>
//#include <Danvil/SimpleEngine/InputHandling/IInputListener.h>
//#include <Danvil/SimpleEngine/Viewport.h>
#include "Common.h"
#include <set>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class CANDY_API Manager
//: public Danvil::SimpleEngine::IRenderable,
//  public Danvil::SimpleEngine::IInputListener
{
public:
	Manager();
	virtual ~Manager();

public:
	void add(const PTR(Widget)& a);

	void remove(const PTR(Widget)& a);

	void SetViewPortSize(unsigned int w, unsigned int h);

public:
	PTR(Widget) pick(const Point2& p);

	bool ateEvent() {
		return didAteEvent;
	}

	void render();

public:
	static void GlVertex(const Point2& p);
	static unsigned int Height;

public:
	void onMousePressed(KeyboardModifiers mod, MouseButton button, int x, int y);
	void onMouseReleased(KeyboardModifiers mod, MouseButton button, int x, int y);
	void onMouseMove(KeyboardModifiers mod, MouseButton buttons, int x, int y);
	void onWheel(KeyboardModifiers mod, int delta);
	void onKeyPressed(KeyboardModifiers mod, int key);
	void onKeyReleased(KeyboardModifiers mod, int key);

private:
	void render(const PTR(Object)& o);
	PTR(Widget) pick(const PTR(Object)& o, const Point2& p);

public:
//	PTR(Danvil::SimpleEngine::Viewport) viewport;

private:
	unsigned int viewport_width_, viewport_height_;
	std::set<PTR(Widget)> windows;

	bool didAteEvent;
	PTR(Widget) heldWidget;
	PTR(Widget) lastWidget;
	Point2 heldStart;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif
