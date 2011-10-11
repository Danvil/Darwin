/*
 * Pipe.cpp
 *
 *  Created on: Oct 11, 2011
 *      Author: david
 */


#include "Pipe.hpp"

namespace Tangerin { namespace Static {

Pipe::Pipe()
: StaticEntity(EntityType::Pipe)
{
}

Pipe::~Pipe()
{
}

void Pipe::Tick(float dt, float total)
{
	StaticEntity::Tick(dt, total);
}

}}
