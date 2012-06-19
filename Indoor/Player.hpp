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

class Player
: public ITickable
{
public:
	Player(Ptr(Candy::DanvilCubes) cubes, Ptr(Candy::Scene) scene);

	void Tick(float dt, float total);

	void OnKeyPressed(Candy::KeyboardModifiers mod, int key);

	void OnKeyReleased(Candy::KeyboardModifiers mod, int key);

	void OnMousePressed(Candy::KeyboardModifiers mod, Candy::MouseButton button, int x, int y);

	void OnMouseReleased(Candy::KeyboardModifiers mod, Candy::MouseButton button, int x, int y);

	void OnMouseMove(Candy::KeyboardModifiers mod, Candy::MouseButton button, int x, int y, int dx, int dy);

private:
	Ptr(Candy::DanvilCubes) cubes_;
	Ptr(Candy::Scene) scene_;
};

#endif
