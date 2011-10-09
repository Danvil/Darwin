/*
 * Creatures.hpp
 *
 *  Created on: Oct 9, 2011
 *      Author: david
 */

#ifndef CREATURES_HPP_
#define CREATURES_HPP_

#include "Creature.hpp"
#include "CreatureRenderGroup.h"

namespace Tangerin
{
	class Creatures
	: public Candy::IDrawable
	{
	public:
		Creatures() {
			crg_.reset(new CreatureRenderGroup("creatures/drone.obj", false));

			crg_->Add(new Creature(Vec3f(0,0,5)));
			crg_->Add(new Creature(Vec3f(3,6,4)));
			crg_->Add(new Creature(Vec3f(-4,6,6)));
		}

		void Render() {
			crg_->Render();
		}

		Ptr(CreatureRenderGroup) crg_;
	};
}

#endif /* CREATURES_HPP_ */
