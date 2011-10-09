/*
 * AutoDrone.cpp
 *
 *  Created on: Oct 9, 2011
 *      Author: david
 */

#include "AutoDrone.hpp"
#include <Candy/Tools/Random.h>

namespace Tangerin { namespace Creatures {

AutoDrone::AutoDrone()
: Creature(CreatureTypes::AutoDrone)
{
	last_time_reached_ = 0.0f;
	height_over_ground_ = 1.4f;
}

void AutoDrone::Tick(float dt, float time)
{
	Creature::Tick(dt, time);
	if(isTargetReachedRecently()) {
		last_time_reached_ = time;
	}
	if(!isMoving()) {
		float time_since_stop = time - last_time_reached_;
		float restart_frequency = 1.0f / 5.0f;
		if(Random::Uniformf() > std::exp(-restart_frequency * time_since_stop)) {
			// set new target
			Vec3f delta = Random::UniformRandomInDisc(10.0f);
			setTarget(position() + delta);
		}
	}
}

}}
