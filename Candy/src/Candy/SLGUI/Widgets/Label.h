/*
 * Label.h
 *
 *  Created on: 29.11.2009
 *      Author: david
 */

#ifndef STARLORD_SLGUI_ELEMENTS_WIDGETS_LABEL_H_
#define STARLORD_SLGUI_ELEMENTS_WIDGETS_LABEL_H_
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Frame.h"
#include <Candy/Engine/Resources/Texture.h>
#include <Candy/Tools/Image3ub.h>
//#include <Danvil/Images/Image.h>
#include <string>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class CANDY_API Label
: public Frame
{
public:
	Label();

	Label(const std::string& text);

	Label(const Image3ub& img);

	Label(const PTR(Candy::Texture2)& tex);

	virtual ~Label();

	std::string getText() const { return text; }

	void setText(const std::string& text);

	void setImage(const Image3ub& img);

	void setTexture(const PTR(Candy::Texture2)& tex);

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
	std::string text;

	PTR(Candy::Texture2) texture_;

	Point2 backgroundSize;

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif
