/*
 * Entity.hpp
 *
 *  Created on: Oct 11, 2011
 *      Author: david
 */

#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <Candy/Tools/LinAlg.h>

namespace Tangerin
{

	namespace EntityTypes
	{
		enum EntityType
		{
			// creatures
			AutoDrone,
			// statics
			Pipe
		};
	}

	typedef EntityTypes::EntityType EntityType;

	class Entity
	{
	public:
		Entity(EntityType type)
		: type_(type) {}

		virtual ~Entity() {}

		EntityType getType() const {
			return type_;
		}

		const Eigen::Affine3f& getPose() const {
			return pose_;
		}

		const Eigen::Matrix4f& getPoseMatrix() const {
				return pose_matrix_;
		}

		void setPose(const Vec3f& position) {
			pose_ = Eigen::Translation3f(position);
			pose_matrix_ = pose_.matrix();
		}

		void setPose(const Vec3f& position, float orientation) {
			pose_ = Eigen::Translation3f(position) * Eigen::AngleAxisf(orientation, Eigen::Vector3f::UnitZ());
			pose_matrix_ = pose_.matrix();
		}

		void setCenterPosition(const Vec3f& center_position) {
			center_position_ = center_position;
		}

		const Vec3f& getCenterPosition() const {
			return center_position_;
		}

		void setLight(const Vec3f& light_ambient, const Vec3f& light_sun) {
			light_ambient_ = light_ambient;
			light_sun_ = light_sun;
		}

		const Vec3f& getLightAmbient() const {
			return light_ambient_;
		}

		const Vec3f& getLightSun() const {
			return light_sun_;
		}

		virtual void Tick(float dt, float time) = 0;

	private:
		/** Entity type */
		EntityType type_;

		/** Entity render pose */
		Eigen::Affine3f pose_;

		/** Entity render pose matrix */
		Eigen::Matrix4f pose_matrix_;

		Vec3f center_position_;

		Vec3f light_ambient_;

		Vec3f light_sun_;

	};

}

#endif
