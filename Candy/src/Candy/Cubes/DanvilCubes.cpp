/*
 * DanvilCubes.cpp
 *
 *  Created on: 25.09.2011
 *      Author: david
 */

#include "DanvilCubes.hpp"
#include <Candy/Cubes/Cell.h>
#include <Candy/Cubes/Impl/Background.h>
#include <Candy/Cubes/Appearance.h>
#include <Candy/Cubes/Rendering/CubesRenderling.h>
#include <Candy/Cubes/Generator.h>
#include <Candy/Cubes/Impl/Physics.hpp>
#include <Candy/Cubes/Rendering/SphereRenderer.hpp>

namespace Candy
{

DanvilCubes::DanvilCubes(const GenerationProperties& gen)
{
	cubes_.reset(new ::Cubes());
	man_.reset(new CubesRenderling(cubes_.get()));
	cubes_->OnAddCell = boost::bind(&DanvilCubes::OnAddCell, this, _1);
	cubes_->OnChangeCube = boost::bind(&DanvilCubes::OnUpdateCube, this, _1, _2);

	height_lookup_.reset(new GroundHeightLookup());
	height_lookup_->SetHeightRange(128);

	sphere_renderer_.reset(new Candy::Cubes::Rendering::SphereRenderer());

	cubes_physics_.reset(new Candy::Cubes::Physics::PhysicsWorld(cubes_, sphere_renderer_));

	background_ = new Background(cubes_, man_, height_lookup_, gen.generator_);
	if(gen.build_first_) {
		background_->enableBuildFirst();
	}
	gen.generator_->PrepareGeneration(cubes_.get(), gen.world_size_);
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
}

void DanvilCubes::OnUpdateCube(Cell* cell, const CoordI& cw)
{
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
