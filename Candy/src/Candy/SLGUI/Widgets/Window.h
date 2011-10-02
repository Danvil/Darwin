/*
 * Window.h
 *
 *  Created on: 28.12.2009
 *      Author: david
 */

#ifndef STARLORD_SLGUI_WIDGETS_WINDOW_H_
#define STARLORD_SLGUI_WIDGETS_WINDOW_H_
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "../Widget.h"
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/** A movable widget with a title */
class CANDY_API Window
:	public Widget
{
public:
	Window();

	virtual ~Window();

	void render();

	void childAdded(const PTR(Object)& a);

public:
	std::string windowTitle;

private:
	boost::signals::connection notifierConnection;

public:
	static PTR(Window) Factor(const std::string& title);

	static PTR(Window) Factor(const std::string& title, float px, float py);

	static PTR(Window) Factor(const std::string& title, float px, float py, float sx, float sy);

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif
