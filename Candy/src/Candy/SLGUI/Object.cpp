/*
 * Object.cpp
 *
 *  Created on: 29.11.2009
 *      Author: david
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Object.h"
#include "Widget.h"
#include "Layout.h"
#include <boost/foreach.hpp>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Object::Object()
:	pos(0, 0),
 	size(0, 0),
 	isVisible(true),
 	takesLayoutSpaceIfHidden(false) {
}

Object::~Object() {
}

bool Object::isWidget() const {
	return dynamic_cast<const Widget*>(this) != 0;
}

PTR(Widget) Object::toWidget() const {
	return std::const_pointer_cast<Widget>(std::dynamic_pointer_cast<const Widget>(shared_from_this()));
}

void Object::addWidget(const PTR(Widget)& w) {
	children.push_back(w);
	w->parent = shared_from_this();
	onAddChild(w);
}

void Object::addLayout(const PTR(Layout)& w) {
	children.push_back(w);
	w->parent = shared_from_this();
	onAddChild(w);
}

Point2 Object::getWorldPosition() const {
	if(hasParent()) {
		return pos + parent->getWorldPosition();
	} else {
		return pos;
	}
}

void Object::show() {
	isVisible = true;
	// if parent is layout recompute
	if(hasParent() && DanvilIsOfType<Layout, Object>(parent)) {
		PTR(Layout) layout = DanvilConvert<Layout, Object>(parent);
		layout->update();
	}
}

void Object::hide() {
	isVisible = false;
	// if parent is layout recompute
	if(hasParent() && DanvilIsOfType<Layout, Object>(parent)) {
		PTR(Layout) layout = DanvilConvert<Layout, Object>(parent);
		layout->update();
	}
}

void Object::setTakesLayoutSpaceIfHidden(bool a) {
	takesLayoutSpaceIfHidden = a;
	// if parent is layout recompute
	if(hasParent() && DanvilIsOfType<Layout, Object>(parent)) {
		PTR(Layout) layout = DanvilConvert<Layout, Object>(parent);
		layout->update();
	}
}


void Object::getChildrenDimensions(Point2& r_pos, Point2& r_size) {
	Point2 min(0, 0);
	Point2 max = size;
	if(hasChildren()) {
		BOOST_FOREACH(const PTR(Object)& a, children) {
			Point2 p, s;
			a->getChildrenDimensions(p, s);
			float x1 = p.x();
			float x2 = p.x() + s.x();
			float y1 = p.y();
			float y2 = p.y() + s.y();
			Danvilifmin(min.x(), x1);
			Danvilifmax(max.x(), x2);
			Danvilifmin(min.y(), y1);
			Danvilifmax(max.y(), y2);
		}
	}
	r_pos = min + pos;
	r_size = max - min;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
