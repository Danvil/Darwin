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
		c.reset(new Creatures::AutoDrone(shared_from_this()));
		break;
	case EntityTypes::Pipe:
		c.reset(new Static::Pipe(shared_from_this()));
		break;
	default:
		throw std::runtime_error("Unknown entity type!");
	};
	if(std::find(registered_types_.begin(), registered_types_.end(), type) == registered_types_.end()) {
		c->Register();
		registered_types_.push_back(type);
	}
	c->Initialize();
	entities_.push_back(c);
	return c;
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

void EntityManager::RegisterRenderInfo(const EntityRenderInfo& info)
{
	if(render_groups_.find(info.tag_) != render_groups_.end()) {
		// already exists
		// TODO check for duplicate?
		return;
	}
	Ptr(EntityRenderInfo) eri = EntityRenderInfoFactory::GetSingleton()->registerInfo(info);
	render_groups_[eri->tag_] = Ptr(EntityRenderGroup)(new EntityRenderGroup(eri));
}

void EntityManager::ChangeRenderInfo(const Ptr(Entity)& entity, const std::string& tag_new, const std::string& tag_old)
{
	if(tag_old == tag_new) {
		return;
	}
	// remove from old group
	if(tag_old != "") {
		auto it = render_groups_.find(tag_old);
		if(it == render_groups_.end()) {
			throw std::runtime_error("Unknown render info tag '" + tag_old + "'!");
		}
		it->second->Remove(entity);
	}
	// add to new group
	if(tag_new != "") {
		auto it = render_groups_.find(tag_new);
		if(it == render_groups_.end()) {
			throw std::runtime_error("Unknown render info tag '" + tag_new + "'!");
		}
		it->second->Add(entity);
	}
}

}

