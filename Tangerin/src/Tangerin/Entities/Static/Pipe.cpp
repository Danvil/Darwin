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
	manager_->RegisterRenderInfo(EntityRenderInfo(
			"pipe_straight",
			"static/pipe_straight.obj",
			"static/pipe_straight_tex_ao.png",
			"static/pipe_straight_tex_emit.png"));
	manager_->RegisterRenderInfo(EntityRenderInfo(
			"pipe_arc",
			"static/pipe_arc.obj",
			"static/pipe_arc_tex_ao.png",
			"static/pipe_arc_tex_emit.png"));
}

void Pipe::Initialize()
{
	manager_->ChangeRenderInfo(shared_from_this(), "pipe_straight");
}

void Pipe::Tick(float dt, float total)
{
	StaticEntity::Tick(dt, total);
}

void Pipe::setCubeCoordinate(const CoordI& cc)
{
	StaticEntity::setCubeCoordinate(cc);
	// check surroundings
	Ptr(StaticEntity) n_p0(manager_->GetEntity(cc + CoordI(+1,0,0)), boost::detail::dynamic_cast_tag());
	Ptr(StaticEntity) n_0p(manager_->GetEntity(cc + CoordI(0,+1,0)), boost::detail::dynamic_cast_tag());
	Ptr(StaticEntity) n_m0(manager_->GetEntity(cc + CoordI(-1,0,0)), boost::detail::dynamic_cast_tag());
	Ptr(StaticEntity) n_0m(manager_->GetEntity(cc + CoordI(0,-1,0)), boost::detail::dynamic_cast_tag());
	if(n_p0 && n_0p) {
		manager_->ChangeRenderInfo(shared_from_this(), "pipe_arc");
	}
	if(n_0p && n_m0) {
		manager_->ChangeRenderInfo(shared_from_this(), "pipe_arc");
	}
	if(n_m0 && n_0m) {
		manager_->ChangeRenderInfo(shared_from_this(), "pipe_arc");
	}
	if(n_0m && n_p0) {
		manager_->ChangeRenderInfo(shared_from_this(), "pipe_arc");
	}
}

}}
