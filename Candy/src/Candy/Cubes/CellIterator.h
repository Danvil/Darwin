/*
 * CellIterator.h
 *
 *  Created on: 20.09.2011
 *      Author: david
 */

#ifndef CELLITERATOR_H_
#define CELLITERATOR_H_

#include "Cell.h"
#include <vector>

struct CellIterator
{
public:
	CellIterator(const std::vector<Cell*>& cells)
	: cells_(cells) {
		it_ = find_next(cells_.begin(), cells_.end());
	}
	operator bool() { return it_ != cells_.end(); }
	Cell* get() const { return *it_; }
	operator Cell*() { return get(); }
	Cell& operator*() { return **it_; }
	Cell* operator->() { return *it_; }
	CellIterator& operator++() {
		it_ = find_next(it_ + 1, cells_.end());
		return *this;
	}
private:
	static std::vector<Cell*>::iterator find_next(std::vector<Cell*>::iterator it, std::vector<Cell*>::iterator end) {
		while(*it == 0 && it != end) ++it;
		return it;
	}
private:
	std::vector<Cell*> cells_;
	std::vector<Cell*>::iterator it_;
};

#endif
