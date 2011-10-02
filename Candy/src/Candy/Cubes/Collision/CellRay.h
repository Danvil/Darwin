#pragma once

#include "GridRayIntersect.h"
#include "AabbRayIntersect.h"
#include <Candy/Cubes/Common.h>
#include <Candy/Tools/Math.h>
#include <Candy/Tools/LinAlg.h>
#include <vector>
#include <map>
#include <cassert>

class Cubes;
struct Cell;

namespace Collision
{
	/** Checks a cell for intersection */
	template<class Tester>
	bool CellRay(const Ci& c_cell, const Vec3f& ray_a, const Vec3f& ray_u, Tester test, Ci& cc_world, float& distance, int& side) {
		float d_start;
		Vec3f cell_min = Properties::CellMin(c_cell);
		Vec3f cell_max = Properties::CellMax(c_cell);
		// find start point
		if(Collision::IsInside(cell_min, cell_max, ray_a)) {
			// we start in the middle of a cell
			d_start = 0.0f;
		}
		else {
			// we start outside of a cell
			if(!Collision::IsIntersecting(cell_min, cell_max, ray_a, ray_u, d_start)) {
				// no intersection -> quit
				return false;
			}
		}
		// check all intersecting cubes and return true if(one of them is not empty
		Ci result;
		Vec3f start = ray_a + d_start * ray_u - cell_min;
		float lambda;
		bool hit = GridRayIntersect(start, ray_u, Ci(0,0,0), Ci(Properties::CellSize,Properties::CellSize,Properties::CellSize), test, result, lambda, side);
		if(hit) {
			cc_world = Properties::CellLocalToWorld(c_cell, CoordU((unsigned int)(result.x), (unsigned int)(result.y), (unsigned int)(result.z)));
			distance = d_start + lambda;
		}
		return hit;
	}

	struct IsSolidTester
	{
		IsSolidTester(Cell* cell)
			: cell_(cell) {}

		bool operator()(int x, int y, int z, float lambda) {
			assert(0 <= x && x < int(Properties::CellSize));
			assert(0 <= y && y < int(Properties::CellSize));
			assert(0 <= z && z < int(Properties::CellSize));
			return IsSolid(cell_->At(Cu(x, y, z)));
		}

	private:
		Cell* cell_;
	};

	/** Checks a cell for intersection */
	bool CellRay(Cell* cell, const Vec3f& ray_a, const Vec3f& ray_u, Ci& cc_world, float& distance, int& side) {
		return CellRay(cell->coordinate(), ray_a, ray_u, IsSolidTester(cell), cc_world, distance, side);
	}
}
