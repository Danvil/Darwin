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
#include <Candy/Tools/Ptr.h>
#include <Candy/Tools/Ticker.hpp>
#include <map>

namespace Candy { class DanvilCubes; }

namespace Tangerin
{
	class EntityManager
	: public Candy::IDrawable, public ITickable
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

	private:
		void Add(const Ptr(Entity)& creature);

	private:
		Ptr(Candy::DanvilCubes) cubes_;

		std::map<EntityType, Ptr(EntityRenderGroup)> render_groups_;

		std::vector<Ptr(Entity)> entities_;

	};
}

#endif
