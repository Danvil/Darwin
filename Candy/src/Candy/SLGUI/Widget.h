/*
 * Widget.h
 *
 *  Created on: 29.11.2009
 *      Author: david
 */

#ifndef STARLORD_SLGUI_WIDGET_H_
#define STARLORD_SLGUI_WIDGET_H_
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Object.h"
#include "Common.h"
//#include <Danvil/SimpleEngine/IRenderable.h>
#include <string>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class CANDY_API Widget
: public Object//,
//  public Danvil::SimpleEngine::IRenderable
{
public:
	Widget();

	virtual ~Widget();

	bool isEmbedded() const {
		return hasParent();
	}

	float opacity;

	Colorf getBackgroundColor() const;

	Colorf getTextColor() const;

	Colorf getHighlightColor() const;

	bool hits(const Point2& p);

	virtual void render();

	friend class Manager;

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif
