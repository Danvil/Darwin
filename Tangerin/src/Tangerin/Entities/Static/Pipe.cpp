/*
 * Pipe.cpp
 *
 *  Created on: Oct 11, 2011
 *      Author: david
 */


#include "Pipe.hpp"

namespace Tangerin { namespace Static {

Pipe::Pipe(const Ptr(EntityServerInterface)& manager)
: StaticEntity(manager, EntityType::Pipe)
{
}

Pipe::~Pipe()
{
}

void Pipe::Register()
{
	manager_->RegisterRenderInfo(EntityRenderInfo("pipe_straight", "static/pipe.obj", "static/pipe_tex_bake_ao.png", "static/pipe_tex_bake_emit.png"));
}

void Pipe::Initialize()
{
	manager_->ChangeRenderInfo(shared_from_this(), "pipe_straight");
}

void Pipe::Tick(float dt, float total)
{
	StaticEntity::Tick(dt, total);
}

}}
