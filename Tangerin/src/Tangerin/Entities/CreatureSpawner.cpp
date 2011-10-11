/*
 * CreatureSpawner.cpp
 *
 *  Created on: Oct 9, 2011
 *      Author: david
 */

#include "CreatureSpawner.hpp"
#include "EntityManager.hpp"
#include "Creature.hpp"
#include <Candy/Tools/Random.h>
#include <algorithm>

namespace Tangerin
{

CreatureSpawner::CreatureSpawner(Ptr(EntityManager) manager, const Vec3f& spawn_position, float frequency)
: manager_(manager), spawn_position_(spawn_position), spawn_frequency_(frequency)
{
}

void CreatureSpawner::Tick(float dt, float total)
{
	std::vector<Ptr(Creature)> remaining(spawned_creatures_.size());
	auto remaining_end = std::copy_if(spawned_creatures_.begin(), spawned_creatures_.end(), remaining.begin(), [](const Ptr(Creature)& c) {
		return c->is_alive();
	});
	spawned_creatures_ = std::vector<Ptr(Creature)>(remaining.begin(), remaining_end);
	if(spawned_creatures_.size() > 10) {
		return;
	}
	if(Random::Uniformf() > std::exp(- spawn_frequency_ * dt)) {
		// spawn
		Ptr(Creature) c = manager_->Add<Creature>(EntityTypes::AutoDrone);
		spawned_creatures_.push_back(c);
		// set position
		c->position() = spawn_position_;
	}
}

}

