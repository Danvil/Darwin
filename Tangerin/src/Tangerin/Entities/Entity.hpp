/*
 * Entity.hpp
 *
 *  Created on: Oct 11, 2011
 *      Author: david
 */

#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include "EntityServerInterface.hpp"
#include <Candy/Tools/LinAlg.h>
#include <Candy/Tools/Ptr.h>
#include <boost/enable_shared_from_this.hpp>

namespace Tangerin { class EntityServerInterface; }

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
	: public boost::enable_shared_from_this<Entity>
	{
	public:
		Entity(const Ptr(EntityServerInterface)& manager, EntityType type)
		: manager_(manager), type_(type) {}

		virtual ~Entity() {}

		EntityType getType() const {
			return type_;
		}

		const std::string& getRenderInfoTag() const {
			return render_info_tag_;
		}

		void setRenderInfoTag(const std::string& tag) {
			render_info_tag_ = tag;
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

		virtual void Register() = 0;

		virtual void Initialize() = 0;

		virtual void Tick(float dt, float time) = 0;

	protected:
		Ptr(EntityServerInterface) manager_;

	private:
		/** Entity type */
		EntityType type_;

		std::string render_info_tag_;

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
