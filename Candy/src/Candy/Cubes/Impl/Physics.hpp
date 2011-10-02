/*
 * Physics.hpp
 *
 *  Created on: 21.09.2011
 *      Author: david
 */

#ifndef PHYSICS_HPP_
#define PHYSICS_HPP_

#include "../Common.h"
#include "../Grid.h"
#include "../Cube.h"
#include <Candy/Tools/LinAlg.h>
#include <Candy/Tools/Ptr.h>
#include <Candy/Tools/Coord.h>
#include <boost/thread.hpp>
#include <algorithm>
#include <vector>

class Cubes;
namespace Candy { namespace Cubes { namespace Rendering { class SphereRenderer; }}}

namespace Candy {
namespace Cubes {
namespace Physics {

struct MovingBlockSphere
{
	MovingBlockSphere(CubeType type, size_t render_id, const Vec3f& position)
	: type_(type), render_id_(render_id), position_(position), velocity_(0,0,0), static_time_(0.0f) {}

	MovingBlockSphere(CubeType type, size_t render_id, const Vec3f& position, const Vec3f& velocity)
	: type_(type), render_id_(render_id), position_(position), velocity_(velocity), static_time_(0.0f) {}

	CubeType type() const {
		return type_;
	}

	size_t render_id() const {
		return render_id_;
	}

	/** Position of the block */
	const Vec3f& position() const {
		return position_;
	}

	/** Radius of the block sphere */
	float radius() const {
		const float cRadius = 0.45f;
		return cRadius;
	}

	bool IsStatic() const {
		return velocity_.norm() < 0.01;
	}

	void ClearContacts() {
		contact_normals_.clear();
	}

	void AddContact(const Vec3f& n) {
		contact_normals_.push_back(n);
	}

	void AddCollision(const Vec3f& p, const Vec3f& n);

	void Tick(float dt);

private:
	CubeType type_;

	size_t render_id_;

	/** Position of the block */
	Vec3f position_;

	/** Velocity of the block */
	Vec3f velocity_;

	/** All contact normals which affect the sphere
	 * As the object is a sphere, the contact normal also gives the contact point.
	 */
	std::vector<Vec3f> contact_normals_;

	float static_time_;

	friend struct PhysicsCell;
	friend struct PhysicsWorld;

};

struct PhysicsCell
{
	CoordI coordinate_world_;
	std::vector<MovingBlockSphere*> blocks_;

	PhysicsCell(const CoordI& c_world)
	: coordinate_world_(c_world) {}

//	void Tick(float dt) {
//		// update spheres
//		std::for_each(blocks_.begin(), blocks_.end(), [&](MovingBlockSphere* block) {
//			block->Tick(dt);
//		});
//	}
};

/** Coordinates the physics simulation */
struct PhysicsWorld
{
	PhysicsWorld(Ptr(::Cubes) cubes, Ptr(::Candy::Cubes::Rendering::SphereRenderer) sphere_renderer);

	~PhysicsWorld();

	/** Updates the simulation */
	void Tick(float dt);

private:
	void StaticThreadMain();

	void CheckCubeCollision(MovingBlockSphere* block);

	void CheckSphereCollision(MovingBlockSphere* a, MovingBlockSphere* b);

	/** Creates a physics block
	 * @param position the center position the created block
	 */
	void CreateBlock(CubeType type, const Vec3f& position);

	void AssignBlock(MovingBlockSphere* block);

	void DeleteBlock(MovingBlockSphere* block);

private:
	Ptr(::Cubes) cubes_;

	Grid<PhysicsCell*> cells_;

	Ptr(Rendering::SphereRenderer) sphere_renderer_;

	boost::mutex mutex_;

	boost::thread* static_thread_;
	bool static_thread_running_;
};

}}}

#endif
