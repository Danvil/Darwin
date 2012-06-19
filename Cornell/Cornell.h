#pragma once

#include "Player.hpp"
#include <Candy/Cubes/DanvilCubes.hpp>
#include <Candy/Engine/IEngine.h>
#include <Candy/Engine/Scene.h>
#include <Candy/Tools/LinAlg.h>
#include <Candy/Tools/Ptr.h>
#include <Candy/Tools/Ticker.hpp>

namespace Candy { class UniformMat4f; }
namespace Candy { class Uniform4f; }

class CornellMain
	: public Candy::IEngine
{
public:
	CornellMain(const std::string& asset_path);
	~CornellMain();

	void Init();

	void Reshape(int width, int height);

	void Render();

	void Update(float dt);

	void OnKeyPressed(Candy::KeyboardModifiers mod, int key);

	void OnKeyReleased(Candy::KeyboardModifiers mod, int key);

	void OnMousePressed(Candy::KeyboardModifiers mod, Candy::MouseButton button, int x, int y);

	void OnMouseReleased(Candy::KeyboardModifiers mod, Candy::MouseButton button, int x, int y);

	void OnMouseMove(Candy::KeyboardModifiers mod, Candy::MouseButton buttons, int x, int y, int dx, int dy);

	void OnWheel(Candy::KeyboardModifiers mod, int delta);

	Ptr(Candy::Scene) scene() const {
		return scene_;
	}

//	extern Cubes* cubes;

//	CubesRenderling* cubes_renderling;

	void Initialize();

private:
	std::string asset_path_;

	Ptr(Player) player_;

	Ptr(Candy::Scene) scene_;

	Ptr(Candy::DanvilCubes) cubes_;

	Ticker ticker_;
};
