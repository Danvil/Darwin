/*
 * StaticEntity.hpp
 *
 *  Created on: Oct 11, 2011
 *      Author: david
 */

#ifndef STATICENTITY_HPP_
#define STATICENTITY_HPP_

#include "Entity.hpp"
#include <Candy/Tools/Coord.h>

namespace Tangerin
{

class StaticEntity
: public Entity
{
public:
	StaticEntity(EntityType type) : Entity(type) {}

	virtual ~StaticEntity() {}

	const CoordI& getCubeCoordinate() const {
		return cube_coordinate_;
	}

	void setCubeCoordinate(const CoordI& cc);

	virtual void Tick(float dt, float total);

protected:
	CoordI cube_coordinate_;

};

}

#endif
