/*
 * StaticEntity.cpp
 *
 *  Created on: Oct 11, 2011
 *      Author: david
 */

#include "StaticEntity.hpp"
#include <Candy/Cubes/Common.h>

namespace Tangerin {

void StaticEntity::setCubeCoordinate(const CoordI& cc)
{
	cube_coordinate_ = cc;
	Vec3f pos = Properties::WorldToPosition(cube_coordinate_);
	setPose(pos);
	Vec3f center_pos = Properties::WorldToPositionCenter(cube_coordinate_);
	setCenterPosition(center_pos);
}

void StaticEntity::Tick(float dt, float total)
{
}

}
