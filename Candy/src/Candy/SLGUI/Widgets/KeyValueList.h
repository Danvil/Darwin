/*
 * KeyValueList.h
 *
 *  Created on: 05.06.2011
 *      Author: david
 */

#ifndef STARLORD_SLGUI_ELEMENTS_WIDGETS_KEYVALUELIST_H_
#define STARLORD_SLGUI_ELEMENTS_WIDGETS_KEYVALUELIST_H_
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Frame.h"
#include <string>
#include <map>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class CANDY_API KeyValueList
: public Frame
{
public:
	KeyValueList();

	KeyValueList(const std::map<std::string,std::string>& items);

	virtual ~KeyValueList();

	const std::map<std::string,std::string>& getItems() const {
		return items;
	}

	void setItems(const std::map<std::string,std::string>& items);

	void setOrder(const std::vector<std::string>& key_order);

	void add(const std::string& key, const std::string& value);

	const std::string& getItemKey(int i);

	const std::string& getItemValue(int i);

	int getSelectionIndex() const {
		return selectionIndex;
	}

	boost::signal<void(int)>& getOnSelectionChangedSignal() {
		return onSelectionChanged;
	}

public:
	void render();

private:
	void recomputeSize();

	void click(const Point2& p);

private:
	std::map<std::string, std::string> items;

	std::vector<std::string> order;

	std::vector<Point2> keyPositions;
	float key_width_;

	int selectionIndex;

	boost::signals::connection c1;

	boost::signal<void(int)> onSelectionChanged;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif
