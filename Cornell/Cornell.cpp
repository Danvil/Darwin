#include "Cornell.h"
#include <Candy/Cubes/Cell.h>
#include <Candy/Cubes/Appearance.h>
#include <Candy/Cubes/Rendering/CubesRenderling.h>
#include <Candy/Cubes/Generator.h>
#include <Candy/Engine/Resources.h>
#include <Candy/Engine/Uniform.h>
#include <Candy/Engine/CoordinateCross.h>
#include <boost/shared_ptr.hpp>
#include <GL/glew.h>
#include <IL/il.h>
#include <cmath>
#include <iostream>
#include <ctime>

CornellMain::CornellMain(const std::string& asset_path)
{
	unsigned int seed = time(0);
	std::cout << "Random generator seed: " << seed << std::endl;
	Random::Seed(seed);

	asset_path_ = asset_path;

	// initialize devil
	ilInit();

	scene_.reset(new Candy::Scene());

	Candy::ResourcePool::Initialize(asset_path_ + "/");

	Appearance::Initialize(asset_path + "/");

	Eigen::Affine3f mv = LinAlg::LookAt(
			Vec3f(0,-50,0),
			Vec3f(0,0,0),
			Vec3f(0,0,1));
	scene_->GetCamera()->SetView(mv);

#ifdef NDEBUG
	const int R = 2;
#else
	const int R = 1;
#endif
	Ptr(Generator) generator = Generators::FactorCornellBox(R);
	WorldSize ws(R, -R, +R);

	Candy::GenerationProperties gen_props;
	gen_props.generator_ = generator;
	gen_props.world_size_ = ws;
	gen_props.build_first_ = true;

	cubes_.reset(new Candy::DanvilCubes(gen_props));
	scene_->Add(cubes_);

	player_.reset(new Player(cubes_, scene_));
	ticker_.Add(player_);
}

CornellMain::~CornellMain()
{
	cubes_.reset();
}

void CornellMain::Init()
{
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClearColor(Appearance::singleton->AmbientColor().x(), Appearance::singleton->AmbientColor().y(), Appearance::singleton->AmbientColor().z(), 1.0f);
}

void CornellMain::Reshape(int width, int height)
{
	scene_->GetCamera()->SetViewport(width, height);
}

void CornellMain::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene_->Render();
}

void CornellMain::Update(float dt)
{
	cubes_->Tick(dt);
	ticker_.Tick(dt);
}

void CornellMain::OnKeyPressed(Candy::KeyboardModifiers mod, int key)
{
	player_->OnKeyPressed(mod, key);
}

void CornellMain::OnKeyReleased(Candy::KeyboardModifiers mod, int key)
{
	player_->OnKeyReleased(mod, key);
}

void CornellMain::OnMousePressed(Candy::KeyboardModifiers mod, Candy::MouseButton button, int x, int y)
{
	player_->OnMousePressed(mod, button, x, y);
}

void CornellMain::OnMouseReleased(Candy::KeyboardModifiers mod, Candy::MouseButton button, int x, int y)
{
	player_->OnMouseReleased(mod, button, x, y);
}

void CornellMain::OnMouseMove(Candy::KeyboardModifiers mod, Candy::MouseButton button, int x, int y, int dx, int dy)
{
	player_->OnMouseMove(mod, button, x, y, dx, dy);
}

void CornellMain::OnWheel(Candy::KeyboardModifiers mod, int delta)
{
}

