/*
 * Object.h
 *
 *  Created on: 29.11.2009
 *      Author: david
 */

#ifndef STARLORD_SLGUI_OBJECT_H_
#define STARLORD_SLGUI_OBJECT_H_
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Common.h"
#include <vector>
#include <boost/enable_shared_from_this.hpp>
#include <boost/signal.hpp>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class Widget;
class Layout;

class CANDY_API Object
: public boost::enable_shared_from_this<Object>
{
public:
	Object();

	virtual ~Object();

	bool hasParent() const {
		return parent;
	}

	bool hasChildren() const {
		return !children.empty();
	}

	bool isWidget() const;

	PTR(Widget) toWidget() const;

	virtual void addWidget(const PTR(Widget)& w);

	virtual void addLayout(const PTR(Layout)& w);

	const PTR(Object)& getParent() const { return parent; }

	const std::vector<PTR(Object)>& getChildren() const { return children; }

	Point2 getWorldPosition() const;

	Point2 getPosition() const { return pos; }

	Point2 getSize() const { return size; }

	void setPosition(const Point2& a) { pos = a; }

	void setSize(const Point2& a) { size = a; }

	bool getIsVisible() const {
		return isVisible;
	}

	bool getTakesLayoutSpaceIfHidden() const {
		return takesLayoutSpaceIfHidden;
	}

	void show();

	void hide();

	void setVisibility(bool a) {
		if(a) {
			show();
		} else {
			hide();
		}
	}

	void setTakesLayoutSpaceIfHidden(bool a);

public:

	void getChildrenDimensions(Point2& pos, Point2& size);

protected:
	Point2 pos;

	Point2 size;

	bool isVisible;

	bool takesLayoutSpaceIfHidden;

private:
	PTR(Object) parent;

	std::vector<PTR(Object)> children;

public:
	boost::signal<void(const PTR(Object)&)> onAddChild;

	boost::signal<void(const PTR(Object)&)> onRemoveChild;

	boost::signal<void(const Point2&)> onMouseDown;

	boost::signal<void(const Point2&)> onMouseUp;

	boost::signal<void()> onMouseEnter;

	boost::signal<void()> onMouseLeave;

	boost::signal<void(int)> onKeyPress;

	friend class Manager;

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif
