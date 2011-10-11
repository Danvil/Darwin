/*
 * Creature.cpp
 *
 *  Created on: Oct 10, 2011
 *      Author: david
 */

#include "Creature.hpp"

namespace Tangerin {

void Creature::Tick(float dt, float time)
{
	is_target_reached_recently_ = false;
	if(has_target_) {
		Vec3f d = target_ - position_;
		d[2] = 0.0f;
		float len = d.norm();
		if(len < 0.1f) {
			has_target_ = false;
			is_target_reached_recently_ = true;
		}
		else {
			position_ += (speed_ / len * dt) * d ;
		}
		setPose(position_);
		setCenterPosition(position_);
	}
}

}

