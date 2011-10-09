/*
 * CreatureManager.cpp
 *
 *  Created on: Oct 9, 2011
 *      Author: david
 */

#include "CreatureManager.hpp"
#include "Creatures/AutoDrone.hpp"
#include <Candy/Cubes/DanvilCubes.hpp>
#include <stdexcept>

namespace Tangerin {

CreatureManager::CreatureManager(const Ptr(Candy::DanvilCubes)& cubes)
: cubes_(cubes)
{
}

Ptr(Creature) CreatureManager::Add(CreatureType type)
{
	Ptr(Creature) c;
	switch(type) {
	case CreatureTypes::AutoDrone:
		c.reset(new Creatures::AutoDrone());
		break;
	default:
		throw std::runtime_error("Unknown creature type!");
	};
	Add(c);
	return c;
}

void CreatureManager::Add(const Ptr(Creature)& creature)
{
	Ptr(CreatureRenderGroup) crg;
	auto it = render_groups_.find(creature->type());
	if(it == render_groups_.end()) {
		std::string fn_mesh, fn_texture;
		switch(creature->type()) {
		case CreatureTypes::AutoDrone:
			fn_mesh = "creatures/drone.obj";
			fn_texture = "creatures/drone_bake_gi.png";
			break;
		default:
			throw std::runtime_error("Unknown creature type!");
		}
		crg.reset(new CreatureRenderGroup(fn_mesh, fn_texture, false));
		render_groups_[creature->type()] = crg;
	}
	else {
		crg = it->second;
	}
	crg->Add(creature);
	creatures_.push_back(creature);
}

void CreatureManager::Render()
{
	for(auto it=render_groups_.begin(); it!=render_groups_.end(); ++it) {
		it->second->Render();
	}
}

void CreatureManager::Tick(float dt, float total)
{
	for(auto it=creatures_.begin(); it!=creatures_.end(); ++it) {
		Ptr(Creature) c = *it;
		float h = cubes_->GetHeight(c->position());
		c->position()[2] = h + c->getHeightOverGround(); // FIX
		c->Tick(dt, total);
	}
}

}
