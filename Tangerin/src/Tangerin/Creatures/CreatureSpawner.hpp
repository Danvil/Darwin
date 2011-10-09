/*
 * CreatureSpawner.hpp
 *
 *  Created on: Oct 9, 2011
 *      Author: david
 */

#ifndef CREATURESPAWNER_HPP_
#define CREATURESPAWNER_HPP_

#include <Candy/Tools/Ptr.h>
#include <Candy/Tools/LinAlg.h>
#include <Candy/Tools/Ticker.hpp>

namespace Tangerin
{

class Creature;
class CreatureManager;

class CreatureSpawner
: public ITickable
{
public:
	CreatureSpawner(Ptr(CreatureManager) manager, const Vec3f& spawn_position, float frequency);

	void Tick(float dt, float total);

private:
	Vec3f spawn_position_;

	float spawn_frequency_;

	Ptr(CreatureManager) manager_;

	std::vector<Ptr(Creature)> spawned_creatures_;
};

}

#endif
