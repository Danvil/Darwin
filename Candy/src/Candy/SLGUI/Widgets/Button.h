/*
 * Button.h
 *
 *  Created on: 06.12.2009
 *      Author: david
 */

#ifndef STARLORD_SLGUI_ELEMENTS_WIDGETS_BUTTON_H_
#define STARLORD_SLGUI_ELEMENTS_WIDGETS_BUTTON_H_
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Frame.h"
#include <string>
#include <boost/signal.hpp>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class CANDY_API Button
: public Frame
{
public:
	Button();

	Button(const std::string& text);

	virtual ~Button();

	std::string getText() const { return text; }

	void setText(const std::string& text);

	boost::signal<void(const Point2&)>& getOnClickSignal() {
		return onMouseUp;
	}

public:
	void render();

	void setPressed(bool a) { isPressed = a; }

private:
	std::string text;

	bool isPressed;

	boost::signals::connection c1, c2, c3;

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif
