/*
 * EntityServerInterface.hpp
 *
 *  Created on: Oct 13, 2011
 *      Author: david
 */

#ifndef ENTITYSERVERINTERFACE_HPP_
#define ENTITYSERVERINTERFACE_HPP_

#include "EntityRenderInfo.hpp"
#include <Candy/Tools/Coord.h>
#include <string>

namespace Tangerin { class Entity; }

namespace Tangerin {

class EntityServerInterface
{
public:
	virtual ~EntityServerInterface() {}

	virtual void RegisterRenderInfo(const EntityRenderInfo& info) = 0;

	virtual void ChangeRenderInfo(const Ptr(Entity)& entity, const std::string& tag_new) = 0;

	virtual Ptr(Entity) GetEntity(const CoordI& coordinate) = 0;

};

}

#endif
