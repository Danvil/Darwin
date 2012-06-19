/*
 * Player.cpp
 *
 *  Created on: 25.09.2011
 *      Author: david
 */

#include "Player.hpp"
#include <Candy/Cubes/Cubes.h>
#include <Candy/Cubes/DanvilCubes.hpp>
#include <Candy/Tools/Coord.h>

Player::Player(Ptr(Candy::DanvilCubes) cubes, Ptr(Candy::Scene) scene)
: cubes_(cubes), scene_(scene)
{
}

void Player::Tick(float dt, float total)
{
}

void Player::OnKeyPressed(Candy::KeyboardModifiers mod, int key)
{
	// camera can move forward and sidewards
	int forward = 0;
	if(key == 'w') forward++;
	if(key == 's') forward--;
	int sidewards = 0;
	if(key == 'a') sidewards--;
	if(key == 'd') sidewards++;
	float speed = 1.0f;
	float forward_amount = speed * float(forward);
	float sidewards_amount = speed * float(sidewards);
	Eigen::Affine3f view = scene_->GetCamera()->GetView();
	Vec3f forward_dir = scene_->GetCamera()->GetForwardDirection();
	Vec3f p = scene_->GetCamera()->GetPosition();
	Vec3f n = cubes_->GetNormal(p);
	if(forward_dir.norm() > 0.0f) {
		view.translation() += forward_amount * forward_dir.normalized();
	}
	Vec3f sidewards_dir = scene_->GetCamera()->GetSidewardsDirection();
	if(sidewards_dir.norm() > 0.0f) {
		view.translation() += sidewards_amount * sidewards_dir.normalized();
	}
	scene_->GetCamera()->SetView(view);
}

void Player::OnKeyReleased(Candy::KeyboardModifiers mod, int key)
{
}

void Player::OnMousePressed(Candy::KeyboardModifiers mod, Candy::MouseButton button, int x, int y)
{

}

void Player::OnMouseReleased(Candy::KeyboardModifiers mod, Candy::MouseButton button, int x, int y)
{
}

void Player::OnMouseMove(Candy::KeyboardModifiers mod, Candy::MouseButton button, int x, int y, int dx, int dy)
{
	// god mode camera
	if( button & Candy::LeftButton ) {
		Eigen::Affine3f view = scene_->GetCamera()->GetView();
		if( mod == Candy::NoModifier ) {
			float amountX = 0.01f * (float)dx;
			float amountY = 0.01f * (float)dy;
			view = LinAlg::RotateAroundTarget(view, amountX, amountY);
		} else if( mod & Candy::ShiftModifier ) {
			float amount = 0.1f * (float)dy;
			view = LinAlg::Zoom(view, amount);
		} else if( mod && Candy::ControlModifier ) {
			float aX = -0.1f * (float)dx;
			float aY = 0.1f * (float)dy;
			view = LinAlg::Pan(view, aX, aY, 0.0f);
		}
		scene_->GetCamera()->SetView(view);
	}
}
