#include "Scene.h"
#include "IDrawable.h"
#include "../Tools/Timer.h"
#include <Eigen/Dense>
#include <iostream>

namespace Candy
{
	#undef GetCurrentTime // ???
	float IDrawable::GetCurrentTime()
	{
		static Danvil::Timer* timer = 0;
		if(!timer) {
			timer = new Danvil::Timer();
			timer->start();
		}
		return float(timer->getElapsedTimeInSec());
	}

	Ptr(Camera) IDrawable::sCamera;

	Scene::Scene()
	{
		camera_.reset(new Camera());
		IDrawable::sCamera = camera_;
	}

	Scene::~Scene()
	{
	}

	void Scene::Render()
	{
		camera_->Apply();
		Node::Render();
	}

}
