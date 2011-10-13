/*
 * EntityRenderGroup.h
 *
 *  Created on: Jun 2, 2011
 *      Author: david
 */

#ifndef CREEPRENDERGROUP_H_
#define CREEPRENDERGROUP_H_

#include "Entity.hpp"
#include "EntityRenderInfo.hpp"
#include <Candy/Engine/Group.h>
#include <Candy/Engine/Shader.h>
#include <Candy/Tools/LinAlg.h>
#include <Candy/Tools/Ptr.h>
#include <string>

namespace Tangerin {

class Creature;

class EntityRenderGroup
	: public Candy::Group<Ptr(Entity)>
{
public:
	EntityRenderGroup(const Ptr(EntityRenderInfo)& eri);

	virtual ~EntityRenderGroup() {}

	void Add(const Ptr(Entity)&);

	void Remove(const Ptr(Entity)&);

	bool TestVisibility(size_t i);

	bool PrepareItem(size_t i, const Ptr(Candy::ShaderX)& shader);

protected:
	void InitializeShader();

public:
	Ptr(EntityRenderInfo) render_info_;
	Candy::Uniform3f* u_sun_position_;
	Candy::Uniform4f* u_object_light_ambient_;
	Candy::Uniform4f* u_object_light_sun_;
};

}
#endif
