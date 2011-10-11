/*
 * CreatureManager.cpp
 *
 *  Created on: Oct 9, 2011
 *      Author: david
 */

#include "EntityManager.hpp"
#include "Creatures/AutoDrone.hpp"
#include "Static/Pipe.hpp"
#include <Candy/Cubes/DanvilCubes.hpp>
#include <stdexcept>

namespace Tangerin {

EntityManager::EntityManager(const Ptr(Candy::DanvilCubes)& cubes)
: cubes_(cubes)
{
}

Ptr(Entity) EntityManager::Add(EntityType type)
{
	Ptr(Entity) c;
	switch(type)
	{
	case EntityTypes::AutoDrone:
		c.reset(new Creatures::AutoDrone());
		break;
	case EntityTypes::Pipe:
		c.reset(new Static::Pipe());
		break;
	default:
		throw std::runtime_error("Unknown entity type!");
	};
	Add(c);
	return c;
}

void EntityManager::Add(const Ptr(Entity)& entity)
{
	Ptr(EntityRenderGroup) crg;
	auto it = render_groups_.find(entity->getType());
	if(it == render_groups_.end()) {
		std::string fn_mesh, fn_tex_ao="", fn_tex_emit="";
		switch(entity->getType()) {
		case EntityTypes::AutoDrone:
			fn_mesh = "creatures/drone.obj";
			fn_tex_ao = "creatures/drone_bake_gi.png";
			break;
		case EntityTypes::Pipe:
			fn_mesh = "static/pipe.obj";
			fn_tex_ao = "static/pipe_tex_bake_ao.png";
			fn_tex_emit = "static/pipe_tex_bake_emit.png";
			break;
		default:
			throw std::runtime_error("Unknown Entity type!");
		}
		crg.reset(new EntityRenderGroup(fn_mesh, fn_tex_ao, fn_tex_emit, false));
		render_groups_[entity->getType()] = crg;
	}
	else {
		crg = it->second;
	}
	crg->Add(entity);
	entities_.push_back(entity);
}

void EntityManager::Render()
{
	for(auto it=render_groups_.begin(); it!=render_groups_.end(); ++it) {
		it->second->Render();
	}
}

void EntityManager::Tick(float dt, float total)
{
	for(auto it=entities_.begin(); it!=entities_.end(); ++it) {
		Ptr(Entity) entity = *it;
		// update lighting values
		float light_ambient, light_sun;
		cubes_->GetLightAtPosition(entity->getCenterPosition(), &light_ambient, &light_sun);
		entity->setLight(light_ambient*Vec3f(1,1,1), light_sun*Vec3f(1,1,1));
		// special udpate for creatures
		Ptr(Creature) creature(entity, boost::detail::dynamic_cast_tag());
		if(creature) {
			float h = cubes_->GetHeight(creature->position());
			creature->position()[2] = h + creature->getHeightOverGround(); // FIX
			creature->Tick(dt, total);
		}
	}
}

}
