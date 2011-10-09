/*
 * CreatureManager.hpp
 *
 *  Created on: Oct 9, 2011
 *      Author: david
 */

#ifndef CREATURES_HPP_
#define CREATURES_HPP_

#include "Creature.hpp"
#include "CreatureRenderGroup.h"
#include <Candy/Tools/Ptr.h>
#include <Candy/Tools/Ticker.hpp>
#include <map>

namespace Candy { class DanvilCubes; }

namespace Tangerin
{
	class CreatureManager
	: public Candy::IDrawable, public ITickable
	{
	public:
		CreatureManager(const Ptr(Candy::DanvilCubes)& cubes);

		Ptr(Creature) Add(CreatureType type);

		void Render();

		void Tick(float dt, float total);

	private:
		void Add(const Ptr(Creature)& creature);

	private:
		Ptr(Candy::DanvilCubes) cubes_;

		std::map<CreatureType, Ptr(CreatureRenderGroup)> render_groups_;

		std::vector<Ptr(Creature)> creatures_;

	};
}

#endif
