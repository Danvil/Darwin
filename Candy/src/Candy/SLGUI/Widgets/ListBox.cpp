/*
 * ListBox.cpp
 *
 *  Created on: 06.12.2009
 *      Author: david
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "ListBox.h"
#include "../Painter.h"
//#include <Danvil/Tools/Small.h>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ListBox::ListBox() {
	recomputeSize();
	borderWidth = 1;
	selectionIndex = -1;
	c1 = onMouseDown.connect(boost::bind(&ListBox::click, this, _1));
}

ListBox::ListBox(const std::vector<std::string>& _items) {
	setItems(_items);
	borderWidth = 1;
	selectionIndex = -1;
	c1 = onMouseDown.connect(boost::bind(&ListBox::click, this, _1));
}

ListBox::~ListBox() {
	c1.disconnect();
}

const float LIST_BOX_PADDING_X = 3.0f;
const float LIST_BOX_PADDING_Y = 2.0f;

void ListBox::setItems(const std::vector<std::string>& _items) {
	items = _items;
	recomputeSize();
}

void ListBox::add(const std::string& s) {
	items.push_back(s);
	recomputeSize();
}

std::string ListBox::getItem(int i) {
	assert(0 <= i && i < int(items.size()));
	return items[i];
}

void ListBox::render() {
	Frame::render();
	if(selectionIndex != -1) {
		Painter::Singleton->setColor(getHighlightColor());
		Painter::Singleton->paintQuad(
				itemPositions[selectionIndex],
				Point2(size.x() - 2*LIST_BOX_PADDING_X, (itemPositions[selectionIndex+1] - itemPositions[selectionIndex]).y()));
	}
	for(unsigned int i=0; i<items.size(); i++) {
		const std::string& s = items[i];
		const Point2& p = itemPositions[i];
		Painter::Singleton->print(Painter::Normal, s, p.x(), p.y() + LIST_BOX_PADDING_Y/2, getTextColor());
	}
}

void ListBox::recomputeSize() {
	itemPositions.clear();
	itemPositions.reserve(items.size());
	Point2 current_pos(LIST_BOX_PADDING_X, LIST_BOX_PADDING_Y);
	size.x() = 0;
	BOOST_FOREACH(const std::string& s, items) {
		float w, h;
		Painter::Singleton->size(Painter::Normal, s, w, h);
		Danvilifmax(size.x(), w);
		itemPositions.push_back(current_pos);
		current_pos.y() += h + LIST_BOX_PADDING_Y;
	}
	itemPositions.push_back(current_pos);
	size.x() += 2 * LIST_BOX_PADDING_X;
	size.y() = current_pos.y() + LIST_BOX_PADDING_Y;
}

void ListBox::click(const Point2& m) {
	if(items.size() == 0) {
		selectionIndex = -1;
	} else {
		for(unsigned int i=0; i<items.size(); i++) {
			const Point2& p = itemPositions[i+1];
			if(m.y() < p.y()) {
				selectionIndex = i;
				break;
			}
		}
	}
	onSelectionChanged(selectionIndex);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
