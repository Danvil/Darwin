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

namespace Tangerin
{

const float cPlayerEyeHeight = 1.67f;
const float cPlayerEyeChange = 0.07f;
const float cPlayerEyePhase = 2.0f * 3.1415f / 1.3f;
const float cStepDeltaMin = 0.35f;
const float cStepDeltaFalloff = 0.70f;

Player::Player(Ptr(Candy::DanvilCubes) cubes, Ptr(Candy::Scene) scene)
: cubes_(cubes), scene_(scene)
{
//	Eigen::Affine3f mv = LinAlg::LookAt(
//			Vec3f(17,33,29),
//			Vec3f(0,0,0),
//			Vec3f(0,0,1));
	Eigen::Affine3f mv = LinAlg::LookAt(
			Vec3f(0,0,32.0f + cPlayerEyeHeight),
			Vec3f(10,0,32.0f + cPlayerEyeHeight),
			Vec3f(0,0,1));
	scene_->GetCamera()->SetView(mv);

	flying_ = false;
	time_ = 0.0f;
	has_step_ = false;
	time_last_step_ = -10.0f;
	is_walking_ = false;
	eye_height_time_ = 0.0f;
	eye_height_ = cPlayerEyeHeight;
	set_cube_ = false;
}

void Player::Tick(float dt)
{
	if(!flying_) {
		time_ += dt;
		if(has_step_) {
			time_last_step_ = time_;
			has_step_ = false;
		}
		float time_since_last_step = time_ - time_last_step_;
		float dynamic_percentage = 1.0f - std::max(0.0f, std::min(1.0f,
				(time_since_last_step - cStepDeltaMin) / (cStepDeltaFalloff - cStepDeltaMin)));
		is_walking_ = (dynamic_percentage > 0);
		if(is_walking_) {
			eye_height_time_ += dt;
		}
		else {
			eye_height_time_ = 0.0f;
		}
		float walk_height_delta = cPlayerEyeChange * std::sin(eye_height_time_ * cPlayerEyePhase);
		float eye_height_old = eye_height_;
		eye_height_ = cPlayerEyeHeight + dynamic_percentage * walk_height_delta;
//		std::cout << eye_height_ << std::endl;
		// position over ground
		Vec3f p = scene_->GetCamera()->GetPosition();
		Vec3f n = cubes_->GetNormal(p);
		p -= eye_height_old * n;
		p = cubes_->GetGroundPoint(p);
		p += eye_height_ * n;
		scene_->GetCamera()->SetPosition(p);
		scene_->GetCamera()->CorrectUp(n);
	}
}

void Player::OnKeyPressed(Candy::KeyboardModifiers mod, int key)
{
	if(key == '1') {
		set_cube_ = false;
	}
	if(key == '2') {
		set_cube_ = true;
	}
	if(key == 'f') {
		flying_ = !flying_;
	}
	// camera can move forward and sidewards
	int forward = 0;
	if(key == 'w') forward++;
	if(key == 's') forward--;
	int sidewards = 0;
	if(key == 'a') sidewards--;
	if(key == 'd') sidewards++;
	float speed = (flying_ ? 1.0f : 0.1f);
	float forward_amount = speed * float(forward);
	float sidewards_amount = speed * float(sidewards);
	Eigen::Affine3f view = scene_->GetCamera()->GetView();
	Vec3f forward_dir = scene_->GetCamera()->GetForwardDirection();
	Vec3f p = scene_->GetCamera()->GetPosition();
	Vec3f n = cubes_->GetNormal(p);
	if(!flying_) {
		float u = n.dot(forward_dir);
		forward_dir -= u * n;
	}
	if(forward_dir.norm() > 0.0f) {
		view.translation() += forward_amount * forward_dir.normalized();
	}
	Vec3f sidewards_dir = scene_->GetCamera()->GetSidewardsDirection();
	if(!flying_) {
		float u = n.dot(sidewards_dir);
		sidewards_dir -= u * n;
	}
	if(sidewards_dir.norm() > 0.0f) {
		view.translation() += sidewards_amount * sidewards_dir.normalized();
	}
	scene_->GetCamera()->SetView(view);
	has_step_ = (forward != 0);
}

void Player::OnKeyReleased(Candy::KeyboardModifiers mod, int key)
{
	// NEVER CALLED!!!
}

void Player::OnMousePressed(Candy::KeyboardModifiers mod, Candy::MouseButton button, int x, int y)
{

}

void Player::OnMouseReleased(Candy::KeyboardModifiers mod, Candy::MouseButton button, int x, int y)
{
	if( button & Candy::LeftButton ) {
		// get ray through pixel
		Vec3f a, u;
		scene_->GetCamera()->PixelRay(x, y, a, u);
		// pick cube
		CoordI cube_cc;
		int cube_side;
		float cube_dist;
		if(cubes_->GetCubes()->Pick(a, u, cube_cc, cube_dist, cube_side)) {
			// add or delete cube
			if(set_cube_) {
				CoordI cw = Properties::GetSideNeighbour(cube_cc, cube_side);
				cubes_->GetCubes()->SetType(cw, CubeTypes::Stone);
			}
			else {
				cubes_->GetCubes()->SetType(cube_cc, CubeTypes::Empty);
			}
		}
	}
}

void Player::OnMouseMove(Candy::KeyboardModifiers mod, Candy::MouseButton button, int x, int y, int dx, int dy)
{
	// god mode camera
//	if( button & Candy::LeftButton ) {
//		Eigen::Affine3f view = scene_->GetCamera()->GetView();
//		if( mod == Candy::NoModifier ) {
//			float amountX = 0.01f * (float)dx;
//			float amountY = 0.01f * (float)dy;
//			view = LinAlg::RotateAroundTarget(view, amountX, amountY);
//		} else if( mod & Candy::ShiftModifier ) {
//			float amount = 0.1f * (float)dy;
//			view = LinAlg::Zoom(view, amount);
//		} else if( mod && Candy::ControlModifier ) {
//			float aX = -0.1f * (float)dx;
//			float aY = 0.1f * (float)dy;
//			view = LinAlg::Pan(view, aX, aY, 0.0f);
//		}
//		scene_->GetCamera()->SetView(view);
//	}

	if( button & Candy::RightButton ) {
		// ego camera
		float amountX = 0.01f * (float)dx;
		float amountY = 0.01f * (float)dy;
		Eigen::Affine3f view = scene_->GetCamera()->GetView();
		view = view * Eigen::AngleAxisf(amountY, Eigen::Vector3f::UnitX());
		Vec3f up = view.linear().inverse() * Vec3f::UnitZ();
		view = view * Eigen::AngleAxisf(amountX, up);
		// correct y-axis
//		view = LinAlg::RotateAroundTarget(view, amountX, amountY);
		scene_->GetCamera()->SetView(view);
	}
}

}
