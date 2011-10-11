/*
 * Creature.hpp
 *
 *  Created on: Oct 9, 2011
 *      Author: david
 */

#ifndef CREATURE_HPP_
#define CREATURE_HPP_

#include "Entity.hpp"
#include <Candy/Tools/LinAlg.h>

namespace Tangerin
{
	class Creature
	: public Entity
	{
	public:
		Creature(EntityType type)
		: Entity(type), height_over_ground_(0.0f), is_alive_(true), health_(100.0f), position_(0.0f, 0.0f, 0.0f), speed_(1.0f), has_target_(false), is_target_reached_recently_(true) {}

		Creature(EntityType type, const Vec3f& p)
		: Entity(type), height_over_ground_(0.0f), is_alive_(true), health_(100.0f), position_(p), speed_(1.0f), has_target_(false), is_target_reached_recently_(true) {}

		virtual ~Creature() {}

		bool is_alive() const {
			return is_alive_;
		}

		bool is_dead() const {
			return !is_alive_;
		}

		float getHeightOverGround() const {
			return height_over_ground_;
		}

		float health() const {
			return health_;
		}

		void damage(float pnts) {
			health_ -= pnts;
			if(health_ < 0.0f) {
				is_alive_ = false;
			}
		}

		void heal(float pnts) {
			if(is_dead()) {
				return;
			}
			health_ += pnts;
			if(health_ > 100.0f) {
				health_ = 100.0f;
			}
		}

		const Vec3f& position() const {
			return position_;
		}

		Vec3f& position() {
			return position_;
		}

		float direction_angle() const {
			return 0.0f;
		}

		void setTarget(const Vec3f& target) {
			has_target_ = true;
			target_ = target;
		}

		bool isMoving() const {
			return has_target_;
		}

		bool isTargetReachedRecently() {
			return is_target_reached_recently_;
		}

		virtual void Tick(float dt, float time);

	protected:
		float height_over_ground_;
		bool is_alive_;
		float health_;
		Vec3f position_;
		float speed_;
		bool has_target_;
		bool is_target_reached_recently_;
		Vec3f target_;
	};
}


#endif
