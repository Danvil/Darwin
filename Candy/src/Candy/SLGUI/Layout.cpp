/*
 * Layout.cpp
 *
 *  Created on: 29.11.2009
 *      Author: david
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Layout.h"
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Layout::Layout() {
	layoutDefinesParentSize = true;
	notifierConnection = onAddChild.connect(boost::bind(&Layout::update, this));
}

Layout::~Layout() {
	notifierConnection.disconnect();
}

void Layout::update() {
	const unsigned int PADDING_X = 6;
	const unsigned int PADDING_Y = 3;
	// position the children in a vertical list
	Point2 curr(PADDING_X, PADDING_Y);
	float x_max = 0;
	BOOST_FOREACH(const PTR(Object)& a, getChildren()) {
		if(!a->getIsVisible() && !a->getTakesLayoutSpaceIfHidden()) {
			continue;
		}
		a->setPosition(curr);
		curr.y() += a->getSize().y() + PADDING_Y;
		x_max = std::max(x_max, a->getSize().x());
	}
	// set all widgets to the same (maximal width)
	BOOST_FOREACH(const PTR(Object)& a, getChildren()) {
		a->setSize(Point2(x_max, a->getSize().y()));
	}
	// resize the layout
	size.y() = curr.y();
	size.x() = x_max + 2*PADDING_X;
	// resize parent so that there is enough room for the layout
	if(getParent()) {
		if(layoutDefinesParentSize) {
			getParent()->setSize(pos + size);
		} else {
			Point2 p_size = getParent()->getSize();
			Danvilifmax(p_size.x(), pos.x() + size.x());
			Danvilifmax(p_size.y(), pos.y() + size.y());
			getParent()->setSize(p_size);
		}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
