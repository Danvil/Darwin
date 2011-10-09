/*
 * CreatureRenderGroup.h
 *
 *  Created on: Jun 2, 2011
 *      Author: david
 */

#ifndef CREEPRENDERGROUP_H_
#define CREEPRENDERGROUP_H_

#include "Creature.hpp"
#include <Candy/Engine/Group.h>
#include <Candy/Engine/Shader.h>
#include <Candy/Tools/LinAlg.h>
#include <Candy/Tools/Ptr.h>
#include <string>

namespace Tangerin {

class Creature;

class CreatureRenderGroup
	: public Candy::Group<Ptr(Creature)>
{
public:
	CreatureRenderGroup(const std::string& fn_mesh, const std::string& fn_texture, bool is_dynamic);

	void Add(const Ptr(Creature)&);

	void Remove(const Ptr(Creature)&);

	bool TestVisibility(size_t i);

	bool PrepareItem(size_t i, const Ptr(Candy::ShaderX)& shader);

protected:
	void InitializeShader();

public:
	std::string fn_texture_;
	Candy::Uniform3f* u_sun_position_;
	Candy::Uniform4f* u_object_light_ambient_;
	Candy::Uniform4f* u_object_light_sun_;
	bool is_dynamic_;
};

}
#endif
