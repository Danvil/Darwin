#pragma once

#include "Player.hpp"
#include "Creatures/Creatures.hpp"
#include <Candy/Cubes/DanvilCubes.hpp>
#include <Candy/Engine/IEngine.h>
#include <Candy/Engine/Scene.h>
#include <Candy/Tools/LinAlg.h>
#include <Candy/Tools/Ptr.h>

namespace Candy { class UniformMat4f; }
namespace Candy { class Uniform4f; }

namespace Tangerin
{
	class TangerinMain
		: public Candy::IEngine
	{
	public:
		TangerinMain(const std::string& asset_path);
		~TangerinMain();

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

		Ptr(Creatures) creatures_;

		Ptr(Candy::Scene) scene_;

		Ptr(Candy::DanvilCubes) cubes_;
	};
}
