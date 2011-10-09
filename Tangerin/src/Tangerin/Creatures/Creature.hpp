/*
 * Creature.hpp
 *
 *  Created on: Oct 9, 2011
 *      Author: david
 */

#ifndef CREATURE_HPP_
#define CREATURE_HPP_

#include <Candy/Tools/LinAlg.h>

namespace Tangerin
{
	class Creature
	{
	public:
		Creature()
		{}

		Creature(const Vec3f& p)
		: position_(p) {}

		const Vec3f& position() const {
			return position_;
		}

		float direction_angle() const {
			return 0.0f;
		}

	private:
		Vec3f position_;
	};
}


#endif
