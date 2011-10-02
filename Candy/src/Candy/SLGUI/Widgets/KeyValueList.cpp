/*
 * KeyValueList.cpp
 *
 *  Created on: 05.06.2011
 *      Author: david
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "KeyValueList.h"
#include "../Painter.h"
#include <boost/bind.hpp>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

KeyValueList::KeyValueList() {
	recomputeSize();
	borderWidth = 1;
	selectionIndex = -1;
	key_width_ = 100;
	c1 = onMouseDown.connect(boost::bind(&KeyValueList::click, this, _1));
}

KeyValueList::KeyValueList(const std::map<std::string,std::string>& _items) {
	setItems(_items);
	borderWidth = 1;
	selectionIndex = -1;
	key_width_ = 100;
	c1 = onMouseDown.connect(boost::bind(&KeyValueList::click, this, _1));
}

KeyValueList::~KeyValueList() {
	c1.disconnect();
}

const float LIST_BOX_PADDING_X = 3.0f;
const float LIST_BOX_PADDING_Y = 2.0f;

void KeyValueList::setItems(const std::map<std::string,std::string>& _items) {
	items = _items;
	recomputeSize();
}

void KeyValueList::add(const std::string& key, const std::string& value) {
	items[key] = value;
	recomputeSize();
}

void KeyValueList::setOrder(const std::vector<std::string>& key_order)
{
	order = key_order;
}

const std::string& KeyValueList::getItemKey(int i) {
	assert(0 <= i && i < int(items.size()));
	return boost::next(items.begin(), i)->first;
}

const std::string& KeyValueList::getItemValue(int i) {
	assert(0 <= i && i < int(items.size()));
	return boost::next(items.begin(), i)->second;
}

void KeyValueList::render() {
	Frame::render();
	if(selectionIndex != -1) {
		Painter::Singleton->setColor(getHighlightColor());
		Painter::Singleton->paintQuad(
				keyPositions[selectionIndex],
				Point2(size.x() - 2*LIST_BOX_PADDING_X, (keyPositions[selectionIndex+1] - keyPositions[selectionIndex]).y()));
	}
	size_t i=0;
	if(order.size() == 0) {
		for(auto it=items.begin(); it!=items.end(); ++it, i++) {
			Painter::Singleton->print(Painter::Normal, it->first, keyPositions[i].x(), keyPositions[i].y() + LIST_BOX_PADDING_Y/2, getTextColor());
			Painter::Singleton->print(Painter::Normal, it->second, keyPositions[i].x() + key_width_, keyPositions[i].y() + LIST_BOX_PADDING_Y/2, getTextColor());
		}
	}
	else {
		for(auto it=order.begin(); it!=order.end(); ++it, i++) {
			Painter::Singleton->print(Painter::Normal, *it, keyPositions[i].x(), keyPositions[i].y() + LIST_BOX_PADDING_Y/2, getTextColor());
			Painter::Singleton->print(Painter::Normal, items[*it], keyPositions[i].x() + key_width_, keyPositions[i].y() + LIST_BOX_PADDING_Y/2, getTextColor());
		}
	}
}

void KeyValueList::recomputeSize() {
	keyPositions.clear();
	keyPositions.reserve(items.size());
	Point2 current_pos(LIST_BOX_PADDING_X, LIST_BOX_PADDING_Y);
	key_width_ = 0;
	float value_width = 0;
	for(std::map<std::string,std::string>::const_iterator it=items.begin(); it!=items.end(); ++it) {
		float w, h;
		Painter::Singleton->size(Painter::Normal, it->first, w, h);
		Danvilifmax(key_width_, w);
		Painter::Singleton->size(Painter::Normal, it->second, w, h);
		Danvilifmax(value_width, w);
		keyPositions.push_back(current_pos);
		current_pos.y() += h + LIST_BOX_PADDING_Y;
	}
	keyPositions.push_back(current_pos);
	key_width_ += 3.0f * LIST_BOX_PADDING_X;
	size.x() = key_width_ + value_width + 2 * LIST_BOX_PADDING_X;
	size.y() = current_pos.y() + LIST_BOX_PADDING_Y;
}

void KeyValueList::click(const Point2& m) {
	if(items.size() == 0) {
		selectionIndex = -1;
	} else {
		for(unsigned int i=0; i<items.size(); i++) {
			const Point2& p = keyPositions[i+1];
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
