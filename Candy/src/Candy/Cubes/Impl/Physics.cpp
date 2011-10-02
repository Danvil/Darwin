/*
 * Physics.cpp
 *
 *  Created on: 23.09.2011
 *      Author: david
 */

#include "Physics.hpp"
#include "../Cubes.h"
#include "../Appearance.h"
#include <Candy/Tools/Random.h>
#include <Candy/Cubes/Rendering/SphereRenderer.hpp>
//#include <boost/timer.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <iostream>

namespace Candy {
namespace Cubes {
namespace Physics {


void MovingBlockSphere::AddCollision(const Vec3f& p, const Vec3f& n)
{
	const float cDamping = 0.25f;
	Vec3f u = position_ - radius() * n;
	float penetration = (u - p).dot(n);
	if(penetration < 0.0f) {
		position_ -= 2.0f * penetration * n;
		velocity_ *= -cDamping;
	}
	AddContact(n);
}

void MovingBlockSphere::Tick(float dt)
{
	// add gravity (gravity is along negative z axis)
	velocity_[2] -= 9.81 * dt;
	// the sphere may not move in negative direction of a contact normal
	for(std::vector<Vec3f>::iterator it=contact_normals_.begin(); it!=contact_normals_.end(); ++it) {
		const Vec3f& n = *it;
		// compute the part parallel to the contact normal
		float a = velocity_.dot(n);
		if(a >= 0.0f) {
			a = 0.0f;
		}
		velocity_ -= a * n;
	}
	// update position
	position_ += dt * velocity_;
}


PhysicsWorld::PhysicsWorld(Ptr(::Cubes) cubes, Ptr(::Candy::Cubes::Rendering::SphereRenderer) sphere_renderer)
: cubes_(cubes), sphere_renderer_(sphere_renderer)
{
	static_thread_running_ = true;
	static_thread_ = new boost::thread(&PhysicsWorld::StaticThreadMain, this);
}

PhysicsWorld::~PhysicsWorld()
{
	static_thread_running_ = false;
	static_thread_->join();
	delete static_thread_;
}

void PhysicsWorld::CreateBlock(CubeType type, const Vec3f& position)
{
	boost::interprocess::scoped_lock<boost::mutex> lock(mutex_);
	// rendering
	Candy::Cubes::Rendering::SphereRenderer::Sphere sphere;
	sphere.position = position;
	sphere.radius = 0.62f;
	sphere.color = Appearance::CubeBaseColor(type);
	size_t rid = sphere_renderer_->Add(sphere);
	// random velocity
	Vec3f rnd_vel = Random::UniformRandomInSphere(0.1f);
	// block
	MovingBlockSphere* block = new MovingBlockSphere(type, rid, position, rnd_vel);
	AssignBlock(block);
}

void PhysicsWorld::StaticThreadMain()
{
	return;
	while(static_thread_running_) {
		// convert all cubes which do not have another cube beneath into moving blocks
		std::vector<Cell*> cells = cubes_->GetCells();
		std::for_each(cells.begin(), cells.end(), [&](Cell* cell) {
			if(!cell->NeedsVitalization() && !cell->IsFullyEmpty()) {
				cell->ApplyToAllCubes([&](CoordI cube_world_coordinate, CubeInteriorData* cube_data) {
					CubeType type = cube_data->GetType();
					if(IsSolid(type)) {
						// check support
						CoordI beneath = cube_world_coordinate;
						beneath.z --;
						if(IsEmpty(cubes_->GetType(beneath))) {
							// delete cube
							cubes_->SetType(cube_world_coordinate, CubeTypes::Empty);
							// create moving block
							CreateBlock(
									type,
									Common::WorldToPositionCenter(cube_world_coordinate));
						}
					}
				});
			}
		});
	}
}

void PhysicsWorld::CheckCubeCollision(MovingBlockSphere* block)
{
	block->ClearContacts();
	const Vec3f& p = block->position();
	Vec3f rest;
	CoordI cw = Common::PositionToInt(p, rest);
	for(unsigned int i=0; i<3; i++) {
		if(rest[i] <= block->radius()) {
			CoordI beneath = cw;
			beneath[i] --;
			if(IsSolid(cubes_->GetType(beneath))) {
				Vec3f c = p;
				c[i] = float(cw[i]);
				Vec3f n = Vec3f::Zero();
				n[i] = +1.0f;
				block->AddCollision(c, n);
			}
		}
		else {
			CoordI above = cw;
			above[i] ++;
			if(IsSolid(cubes_->GetType(above))) {
				Vec3f c = p;
				c[i] = float(cw[i]) + 1.0f;
				Vec3f n = Vec3f::Zero();
				n[i] = -1.0f;
				block->AddCollision(c, n);
			}
		}
	}
}

void PhysicsWorld::CheckSphereCollision(MovingBlockSphere* a, MovingBlockSphere* b)
{
	Vec3f d = b->position() - a->position();
	float d2 = d.squaredNorm();
	float r_sum = a->radius() + b->radius();
	float d2_min = r_sum * r_sum;
	if(d2 < d2_min) {
		// the spheres collide
		// normal
		float len_inv = 1.0f / std::sqrt(d2); // TODO use trick
		Vec3f n = len_inv * d;
		// collision point
		Vec3f c = a->position() + (a->radius() / r_sum) * d; // TODO can do faster if ra==rb
		// add collisions
		a->AddCollision(c, -n); // normal points from b to a
		b->AddCollision(c, n); // normal points from a to b
	}
}

void PhysicsWorld::Tick(float dt)
{
//	static float counter = 0.0f;
//	counter += dt;
//	if(counter < 3.0f) {
//		return;
//	}
////	return;

	boost::interprocess::scoped_lock<boost::mutex> lock(mutex_);

	// get active cells
	// FIXME use active instead of valid cells
	std::vector<PhysicsCell*> active_cells = cells_.valid_cells();
	// get all spheres in active cells
	std::vector<MovingBlockSphere*> active_blocks;
	std::for_each(active_cells.begin(), active_cells.end(), [&](PhysicsCell* cell) {
		active_blocks.insert(active_blocks.begin(), cell->blocks_.begin(), cell->blocks_.end());
	});
//	std::cout << "Active blocks: " << active_blocks.size() << std::endl;
	// compute collisions and contact normals
	std::for_each(active_blocks.begin(), active_blocks.end(), [&](MovingBlockSphere* block) {
		CheckCubeCollision(block);
	});
	for(size_t i=0; i<active_blocks.size(); i++) {
		for(size_t j=i+1; j<active_blocks.size(); j++) {
			MovingBlockSphere* a = active_blocks[i];
			MovingBlockSphere* b = active_blocks[j];
			CheckSphereCollision(a, b);
		}
	}
	// FIXME inter sphere collision
	// move spheres
	std::for_each(active_blocks.begin(), active_blocks.end(), [&](MovingBlockSphere* block) {
		block->Tick(dt);
		sphere_renderer_->Get(block->render_id()).position = block->position();
	});
	// convert static moving blocks to static blocks
	std::vector<MovingBlockSphere*> not_deleted;
	not_deleted.reserve(active_blocks.size());
	std::for_each(active_blocks.begin(), active_blocks.end(), [&](MovingBlockSphere* block) {
		bool deleted = false;
		if(block->IsStatic()) {
			block->static_time_ += dt;
			//if(block->static_time_ > 0.5f) { // FIXME ??
				DeleteBlock(block);
				deleted = true;
			//}
		}
		else {
			block->static_time_ = 0.0f;
		}
		if(!deleted) {
			not_deleted.push_back(block);
		}
	});
	// re-distribute spheres to correct cells
	std::for_each(active_cells.begin(), active_cells.end(), [](PhysicsCell* cell) {
		cell->blocks_.clear();
	});
	std::for_each(not_deleted.begin(), not_deleted.end(), [&](MovingBlockSphere* block) {
		this->AssignBlock(block);
	});
}

void PhysicsWorld::AssignBlock(MovingBlockSphere* block)
{
	// block
	CoordI cw = Common::PositionToInt(block->position());
	CoordI cc = Common::WorldToCell(cw);
	PhysicsCell* cell = this->cells_.get(cc, [&]() { return new PhysicsCell(cc); });
	cell->blocks_.push_back(block);
}

void PhysicsWorld::DeleteBlock(MovingBlockSphere* block)
{
	// convert to cube
	CoordI cw = Common::PositionToInt(block->position());
	cubes_->SetType(cw, block->type());
	// delete from rendering
	sphere_renderer_->Remove(block->render_id());
}

}}}
