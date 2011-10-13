/*
 * AutoDrone.cpp
 *
 *  Created on: Oct 9, 2011
 *      Author: david
 */

#include "AutoDrone.hpp"
#include <Candy/Tools/Random.h>

namespace Tangerin { namespace Creatures {

AutoDrone::AutoDrone(const Ptr(EntityServerInterface)& manager)
: Creature(manager, EntityTypes::AutoDrone)
{
	last_time_reached_ = 0.0f;
	height_over_ground_ = 1.4f;
}

void AutoDrone::Register()
{
	manager_->RegisterRenderInfo(EntityRenderInfo("auto_drone", "creatures/drone.obj", "creatures/drone_bake_gi.png"));
}

void AutoDrone::Initialize()
{
	manager_->ChangeRenderInfo(shared_from_this(), "auto_drone");
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
