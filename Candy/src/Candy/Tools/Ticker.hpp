/*
 * Ticker.hpp
 *
 *  Created on: Oct 10, 2011
 *      Author: david
 */

#ifndef TICKER_HPP_
#define TICKER_HPP_

#include "Ptr.h"
#include <list>

class ITickable
{
public:
	virtual ~ITickable() {}
	virtual void Tick(float dt, float total) = 0;
};

class Ticker
{
public:
	Ticker() {}

	void Add(const Ptr(ITickable)& item) {
		items_.push_back(item);
	}

	void Remove(const Ptr(ITickable)& item) {
		auto it = std::find(items_.begin(), items_.end(), item);
		items_.erase(it);
	}

	void Tick(float dt) {
		static float total = 0.0f;
		if(dt > 0.1) {
			dt = 0.1f;
		}
		total += dt;
		for(auto it=items_.begin(); it!=items_.end(); ++it) {
			(*it)->Tick(dt, total);
		}
	}

private:
	std::list<Ptr(ITickable)> items_;
};

#endif
