/*
 * TextBox.h
 *
 *  Created on: 29.11.2009
 *      Author: david
 */

#ifndef STARLORD_SLGUI_ELEMENTS_WIDGETS_TEXTBOX_H_
#define STARLORD_SLGUI_ELEMENTS_WIDGETS_TEXTBOX_H_
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Frame.h"
#include <string>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class CANDY_API TextBox
: public Frame
{
public:
	TextBox();

	TextBox(const std::string& text);

	virtual ~TextBox();

	std::string getText() const { return text; }

	void setText(const std::string& text);

	void render();

public:
	enum EHOrientation {
		Left, Center, Right
	};

	enum EVOrientation {
		Up, Mid, Bottom
	};

	EHOrientation horizontalAlignment;

	EVOrientation verticalAlignment;

private:
	void AddKey(int key);

	boost::signals::connection c1;

private:
	std::string text;

	Point2 backgroundSize;

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif
