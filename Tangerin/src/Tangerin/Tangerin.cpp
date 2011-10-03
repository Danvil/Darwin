#include "Tangerin.h"
#include <Candy/Cubes/Cell.h>
#include <Candy/Cubes/Background.h>
#include <Candy/Cubes/Appearance.h>
#include <Candy/Cubes/Rendering/CubesRenderling.h>
#include <Candy/Cubes/Generator.h>
#include <Candy/Cubes/Impl/Physics.hpp>
#include <Candy/Cubes/Rendering/SphereRenderer.hpp>
#include <Candy/Engine/Resources.h>
#include <Candy/Engine/Uniform.h>
#include <Candy/Engine/CoordinateCross.h>
#include <Candy/Engine/Tests/Crate.h>
#include <Candy/Engine/Tests/Fractal.h>
#include <Candy/Tools/Perlin.h>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/random.hpp>
#include <boost/thread.hpp>
#include <GL/glew.h>
#include <IL/il.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <ctime>

void GenerateWorld(Ptr(Cubes) cubes)
{
#ifdef NDEBUG
	const int cCellRange = 8;
	const int cCellRangeZMin = -1;
	const int cCellRangeZMax = +1;
#else
	const int cCellRange = 2;
	const int cCellRangeZMin = -1;
	const int cCellRangeZMax = +1;
#endif
//	Ptr(Generator) gen = Generators::FactorZeroHeight();
//	Ptr(Generator) gen = Generators::FactorPerlin3D();
//	Ptr(Generator) gen = Generators::FactorHillsAndFly();
//	Ptr(Generator) gen = Generators::FactorPerlinHills();
	Ptr(Generator) gen = Generators::FactorSphericalTerra();
//	Ptr(Generator) gen = Generators::FactorTerra();
//	WorldSize ws(cCellRange, cCellRangeZMin, cCellRangeZMax);
	WorldSize ws;
	ws.x1 = 0;
	ws.x2 = 64;
	ws.y1 = -16;
	ws.y2 = +16;
	ws.z1 = -1;
	ws.z2 = +1;
	gen->Generate(cubes.get(), ws);
}

#include <Candy/SLGUI/Manager.h>
#include <Candy/SLGUI/Scheme.h>
Ptr(Candy::SLGUI::Manager) manager;

namespace Tangerin
{

TangerinMain::TangerinMain(const std::string& asset_path)
{
	unsigned int seed = time(0);
//	seed = 313;
//	seed = 0;
	std::cout << "Random generator seed: " << seed << std::endl;
	Random::Seed(seed);
	// initialize perlin noise
	Perlin::Initialize(seed);

	asset_path_ = asset_path;

	Candy::SLGUI::Scheme::LoadDefault(asset_path_);

	// initialize devil
	ilInit();

	scene_.reset(new Candy::Scene());

	Candy::ResourcePool::Initialize(asset_path_ + "/");

	Appearance::Load(asset_path + "/");

	Eigen::Affine3f mv = LinAlg::LookAt(
			Vec3f(17,33,29),
			Vec3f(0,0,0),
			Vec3f(0,0,1));
	scene_->GetCamera()->SetView(mv);

	//scene_->Add(new Candy::Crate());
	//scene_->Add(Candy::Fractal::ConstructSierpinskiCubes(4));

	cubes_.reset(new Candy::DanvilCubes());
	scene_->Add(cubes_);

//	cubes_.reset(new Cubes());
//	man_.reset(new CubesRenderling(cubes_.get()));
//	cubes_->OnAddCell = boost::bind(&CubesRenderling::NotifyAddCell, man_, _1);
//	cubes_->OnChangeCell = boost::bind(&CubesRenderling::NotifyInvalidate, man_, _1);
//	scene_->Add(man_);

	GenerateWorld(cubes_->GetCubes());

//	sphere_renderer_.reset(new Candy::Cubes::Rendering::SphereRenderer());
//	scene_->Add(sphere_renderer_);
//
//	cubes_physics_.reset(new Candy::Cubes::Physics::PhysicsWorld(cubes_, sphere_renderer_));

	player_.reset(new Player(cubes_, scene_));

//	background_ = new Background(cubes_, man_);
//	background_->Start();

	manager.reset(new Candy::SLGUI::Manager());
}

TangerinMain::~TangerinMain()
{
}

void TangerinMain::Init()
{
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClearColor(Appearance::AmbientColor.x(), Appearance::AmbientColor.y(), Appearance::AmbientColor.z(), 1.0f);

//	man_->UpdateMeshAll();
}

void TangerinMain::Reshape(int width, int height)
{
	scene_->GetCamera()->SetViewport(width, height);
	manager->SetViewPortSize(width, height);
//	// change viewport to full window area
//	glViewport(0, 0, width, height);
//	// change projection matrix
//	Mat4f proj = LinAlg::ProjectionMatrix(float(height) / float(width), 0.1f, 1000.0f);
//	scene_->ChangeProjectionMatrix(proj);
}

void TangerinMain::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene_->Render();
	manager->render();
}

void TangerinMain::Update(float dt)
{
	if(dt > 0.1) {
		dt = 0.1f;
	}
	cubes_->Tick(dt);
	player_->Tick(dt);
}

void TangerinMain::OnKeyPressed(Candy::KeyboardModifiers mod, int key)
{
	manager->onKeyPressed(mod, key);
	player_->OnKeyPressed(mod, key);
}

void TangerinMain::OnKeyReleased(Candy::KeyboardModifiers mod, int key)
{
	// never called!
	manager->onKeyReleased(mod, key);
	player_->OnKeyReleased(mod, key);
}

void TangerinMain::OnMousePressed(Candy::KeyboardModifiers mod, Candy::MouseButton button, int x, int y)
{
	manager->onMousePressed(mod, button, x, y);
	if(manager->ateEvent()) {
		return;
	}
	player_->OnMousePressed(mod, button, x, y);
}

void TangerinMain::OnMouseReleased(Candy::KeyboardModifiers mod, Candy::MouseButton button, int x, int y)
{
	manager->onMouseReleased(mod, button, x, y);
	player_->OnMouseReleased(mod, button, x, y);
}

void TangerinMain::OnMouseMove(Candy::KeyboardModifiers mod, Candy::MouseButton button, int x, int y, int dx, int dy)
{
	manager->onMouseMove(mod, button, x, y);
	if(manager->ateEvent()) {
		return;
	}
	player_->OnMouseMove(mod, button, x, y, dx, dy);
}

void TangerinMain::OnWheel(Candy::KeyboardModifiers mod, int delta)
{
	manager->onWheel(mod, delta);
}

}
