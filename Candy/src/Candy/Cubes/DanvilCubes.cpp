/*
 * DanvilCubes.cpp
 *
 *  Created on: 25.09.2011
 *      Author: david
 */

#include "DanvilCubes.hpp"
#include <Candy/Cubes/Cell.h>
#include <Candy/Cubes/Background.h>
#include <Candy/Cubes/Appearance.h>
#include <Candy/Cubes/Rendering/CubesRenderling.h>
#include <Candy/Cubes/Generator.h>
#include <Candy/Cubes/Impl/Physics.hpp>
#include <Candy/Cubes/Rendering/SphereRenderer.hpp>

namespace Candy
{

DanvilCubes::DanvilCubes()
{
	cubes_.reset(new ::Cubes());
	man_.reset(new CubesRenderling(cubes_.get()));
	cubes_->OnAddCell = boost::bind(&DanvilCubes::OnAddCell, this, _1);
	cubes_->OnChangeCell = boost::bind(&DanvilCubes::OnUpdateCell, this, _1, _2);
	cubes_->OnChangeCube = boost::bind(&DanvilCubes::OnUpdateCube, this, _1, _2);

	height_lookup_.SetHeightRange(128);

	sphere_renderer_.reset(new Candy::Cubes::Rendering::SphereRenderer());

	cubes_physics_.reset(new Candy::Cubes::Physics::PhysicsWorld(cubes_, sphere_renderer_));

	background_ = new Background(cubes_, man_);
	background_->Start();
}

DanvilCubes::~DanvilCubes()
{
	background_->Stop();
	delete background_;
}

void DanvilCubes::OnAddCell(Cell* cell)
{
	man_->NotifyAddCell(cell);
	height_lookup_.Build(cubes_, cell);
}

void DanvilCubes::OnUpdateCell(Cell* cell, bool first)
{
	man_->NotifyInvalidate(cell);
	if(first) {
		height_lookup_.Build(cubes_, cell);
	}
}

void DanvilCubes::OnUpdateCube(Cell* cell, const CoordI& cw)
{
	man_->NotifyInvalidate(cell);
	height_lookup_.Build(cubes_, cw.x, cw.y);
}

void DanvilCubes::Tick(float dt)
{
	cubes_physics_->Tick(dt);
}

void DanvilCubes::Render()
{
	man_->Render();
	sphere_renderer_->Render();
}

}
