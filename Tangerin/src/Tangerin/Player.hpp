/*
 * Player.hpp
 *
 *  Created on: 25.09.2011
 *      Author: david
 */

#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include <Candy/Engine/IEngine.h>
#include <Candy/Engine/Scene.h>
#include <Candy/Tools/LinAlg.h>
#include <Candy/Tools/Ticker.hpp>

namespace Candy { class DanvilCubes; }
namespace Tangerin { class EntityManager; }

namespace Tangerin
{
	class Player
	: public ITickable
	{
	public:
		Player(Ptr(Candy::DanvilCubes) cubes, Ptr(EntityManager) entity_manager, Ptr(Candy::Scene) scene);

		void Tick(float dt, float total);

		void OnKeyPressed(Candy::KeyboardModifiers mod, int key);

		void OnKeyReleased(Candy::KeyboardModifiers mod, int key);

		void OnMousePressed(Candy::KeyboardModifiers mod, Candy::MouseButton button, int x, int y);

		void OnMouseReleased(Candy::KeyboardModifiers mod, Candy::MouseButton button, int x, int y);

		void OnMouseMove(Candy::KeyboardModifiers mod, Candy::MouseButton button, int x, int y, int dx, int dy);

	private:
		Ptr(Candy::DanvilCubes) cubes_;
		Ptr(EntityManager) entity_manager_;
		Ptr(Candy::Scene) scene_;
		bool flying_;
		bool has_step_;
		float time_last_step_;
		bool is_walking_;
		float eye_height_time_;
		float eye_height_;
		int cube_edit_mode_;
	};
}

#endif
