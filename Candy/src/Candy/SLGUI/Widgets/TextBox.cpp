/*
 * TextBox.cpp
 *
 *  Created on: 29.11.2009
 *      Author: david
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "TextBox.h"
#include "../Scheme.h"
#include "../Painter.h"
#include <boost/bind.hpp>
#include <iostream>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TextBox::TextBox() {
	horizontalAlignment = Left;
	verticalAlignment = Mid;
	setText("");
	borderWidth = 0;
	c1 = onKeyPress.connect(boost::bind(&TextBox::AddKey, this, _1));
}

TextBox::TextBox(const std::string& _text) {
	horizontalAlignment = Left;
	verticalAlignment = Mid;
	setText(_text);
	borderWidth = 0;
}

TextBox::~TextBox() {
	c1.disconnect();
}

void TextBox::setText(const std::string& _text) {
	text = _text;
	Painter::Singleton->size(Painter::Normal, text, size.x(), size.y());
	size.x() += 6;
	size.y() += 4;
}

void TextBox::render() {
	// render border
	Painter::Singleton->setColor(Scheme::Default()->colorBorder);
	Painter::Singleton->paintQuadBorder(size, 1);
	Painter::Singleton->setColor(Scheme::Default()->colorControlLight);
	Painter::Singleton->paintQuad(size);
	// render text
	if(!text.empty()) {
		Point2 text_dim = Painter::Singleton->size(Painter::Normal, text);
		float px = 0.0f;
		switch(horizontalAlignment) {
		case Left:
			px = 2;
			break;
		case Center:
			px = 0.5f*(size.x() - text_dim.x());
			break;
		case Right:
			px = size.x() - text_dim.x() - 2;
			break;
		}
		float py = 0.0f;
		switch(verticalAlignment) {
		case Up:
			py = 2;
			break;
		case Center:
			py = 0.5f*(size.y() - text_dim.y());
			break;
		case Bottom:
			py = size.y() - text_dim.y() - 2;
			break;
		}
		Painter::Singleton->print(Painter::Normal, text, px, py, getTextColor());
	}
}

void TextBox::AddKey(int key)
{
	if(33 <= key && key <= 126) { 
		text += (unsigned char)key;
		setText(text);
	}
	else if(key == 8) {
		text.erase(text.length() - 1);
		setText(text);
	}
	else {
		std::cout << "Unknown key: " << key << std::endl;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
