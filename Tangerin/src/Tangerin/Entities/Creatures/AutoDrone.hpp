/*
 * AutoDrone.hpp
 *
 *  Created on: Oct 9, 2011
 *      Author: david
 */

#ifndef AUTODRONE_HPP_
#define AUTODRONE_HPP_

#include "../Creature.hpp"

namespace Tangerin { namespace Creatures {

class AutoDrone
: public Creature
{
public:
	AutoDrone(const Ptr(EntityServerInterface)& manager);

	virtual ~AutoDrone() {}

	void Register();

	void Initialize();

	void Tick(float dt, float time);

private:
	float last_time_reached_;
};

}}

#endif
