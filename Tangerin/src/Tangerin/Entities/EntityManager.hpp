/*
 * CreatureManager.hpp
 *
 *  Created on: Oct 9, 2011
 *      Author: david
 */

#ifndef CREATURES_HPP_
#define CREATURES_HPP_

#include "Creature.hpp"
#include "EntityRenderGroup.h"
#include "EntityServerInterface.hpp"
#include <Candy/Tools/Ptr.h>
#include <Candy/Tools/Ticker.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <map>

namespace Candy { class DanvilCubes; }

namespace Tangerin
{
	class EntityManager
	: public Candy::IDrawable,
	  public ITickable,
	  public EntityServerInterface,
	  public boost::enable_shared_from_this<EntityManager>
	{
	public:
		EntityManager(const Ptr(Candy::DanvilCubes)& cubes);

		Ptr(Entity) Add(EntityType type);

		template<typename T>
		Ptr(T) Add(EntityType type) {
			Ptr(Entity) e = Add(type);
			Ptr(T) p(e, boost::detail::dynamic_cast_tag());
			BOOST_ASSERT(p);
			return p;
		}

		void Render();

		void Tick(float dt, float total);

		void RegisterRenderInfo(const EntityRenderInfo& info);

		void ChangeRenderInfo(const Ptr(Entity)& entity, const std::string& tag_new, const std::string& tag_old="");

	private:
		Ptr(Candy::DanvilCubes) cubes_;

		std::map<std::string, Ptr(EntityRenderGroup)> render_groups_;

		std::vector<Ptr(Entity)> entities_;

		std::vector<EntityType> registered_types_;

	};
}

#endif
