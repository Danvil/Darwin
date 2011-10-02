/*
 * DanvilCubes.hpp
 *
 *  Created on: 25.09.2011
 *      Author: david
 */

#ifndef DANVILCUBES_HPP_
#define DANVILCUBES_HPP_

#include "Common.h"
#include "Impl/GroundHeightLookup.h"
#include <Candy/Engine/IDrawable.h>
#include <Candy/Tools/Ptr.h>

class Cubes;
struct Cell;
namespace Candy { namespace Cubes { namespace Physics { class PhysicsWorld; }}}
namespace Candy { namespace Cubes { namespace Rendering { class SphereRenderer; }}}
class CubesRenderling;
class Background;

namespace Candy
{

	class DanvilCubes
	: public Candy::IDrawable
	{
	public:
		DanvilCubes();

		virtual ~DanvilCubes();

		void Tick(float dt);

		void Render();

		Ptr(::Cubes) GetCubes() const {
			return cubes_;
		}

		int GetHeight(int cx, int cy, float z) {
			return height_lookup_.GetGroundZ(cx, cy, z);
		}

		float GetHeight(const Vec3f& position) {
			CoordI c_world = Properties::PositionToWorld(position);
			return float(height_lookup_.GetGroundZ(c_world.x, c_world.y, position[2]));
		}

	private:
		void OnAddCell(Cell* cell);

		void OnUpdateCell(Cell* cell, bool first);

		void OnUpdateCube(Cell* cell, const CoordI& cw);

	private:
		Ptr(::Cubes) cubes_;

		Ptr(Candy::Cubes::Physics::PhysicsWorld) cubes_physics_;

		Ptr(CubesRenderling) man_;

		Ptr(Candy::Cubes::Rendering::SphereRenderer) sphere_renderer_;

		Background* background_;

		GroundHeightLookup height_lookup_;

	};

}

#endif
