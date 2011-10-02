/*
 * ListBox.h
 *
 *  Created on: 06.12.2009
 *      Author: david
 */

#ifndef STARLORD_SLGUI_ELEMENTS_WIDGETS_LISTBOX_H_
#define STARLORD_SLGUI_ELEMENTS_WIDGETS_LISTBOX_H_
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Frame.h"
#include <string>
#include <vector>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class CANDY_API ListBox
: public Frame
{
public:
	ListBox();

	ListBox(const std::vector<std::string>& items);

	virtual ~ListBox();

	const std::vector<std::string>& getItems() const { return items; }

	void setItems(const std::vector<std::string>& items);

	void add(const std::string& s);

	std::string getItem(int i);

	const int& getSelectionIndex() const { return selectionIndex; }

	boost::signal<void(int)>& getOnSelectionChangedSignal() { return onSelectionChanged; }

public:
	void render();

private:
	void recomputeSize();

	void click(const Point2& p);

private:
	std::vector<std::string> items;

	std::vector<Point2> itemPositions;

	int selectionIndex;

	boost::signals::connection c1;

	boost::signal<void(int)> onSelectionChanged;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif
