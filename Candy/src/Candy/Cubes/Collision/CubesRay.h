#pragma once

#include "CellRay.h"
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
	namespace Private
	{
		struct CellTester
		{
			CellTester(Cubes* cubes, const Vec3f& a, const Vec3f& u)
			: cubes_(cubes), a_(a), u_(u) {}

			bool operator()(int x, int y, int z, float lambda) {
				Cell* cell;
				Ci c_cell(x, y, z);
				if(cubes_->TryGetCell(c_cell, &cell)) {
					hits_ = CellRay(c_cell, a_, u_, IsSolidTester(cell), cc_world_, distance_, side_);
					return hits_;
				} else {
					hits_ = false; // did not hit anything, but hit unkown area
					return true; // need to quit!
				}
			}

			bool hits() const { return hits_; }
			const Ci& cc_world() const { return cc_world_; }
			float distance() const { return distance_; }
			int side() const { return side_; }

		private:
			Cubes* cubes_;
			Vec3f a_, u_;
			bool hits_;
			Ci cc_world_;
			float distance_;
			int side_;
		};
	}

	///** Intersection by walking cell by cell in the direction of the ray and checking each cell for intersection */
	//bool CubesRayCellWalk(Cubes* cubes, const Vec3f& a, const Vec3f& u, Ci& min_c_world, float& min_distance, int& side)
	//{
	//	Private::CellTester ct(cubes, a, u);
	//	Vec3f a_cell = (1.0f / float(Properties::CellSize)) * a;
	//	Collision::GridRayIntersect(a_cell, u, ct, min_c_world, min_distance, side);
	//	if(ct.hits()) {
	//		min_c_world = ct.cc_world();
	//		min_distance = ct.distance();
	//		side = ct.side();
	//		return true;
	//	} else {
	//		return false;
	//	}
	//}

	namespace Private
	{
		/** Iteratively checks a set of cells for intersection with a ray and saves the nearest cube which intersects the ray */
		struct CellPicking
		{
			CellPicking(const Vec3f& a, const Vec3f& u, Ci& min_c_world, float& min_distance, int& side)
				: a(a), u(u), min_hits(false), min_c_world(min_c_world), min_distance(min_distance), side(side) {}
		
			void operator()(Cell* cell) {
				Ci c_world;
				float distance;
				bool hits = CellRay(cell->coordinate(), a, u, IsSolidTester(cell), c_world, distance, side);
				if(hits && (!min_hits || distance < min_distance)) {
					min_distance = distance;
					min_c_world = c_world;
					min_hits = hits;
				}
			}

			bool hits() {
				return min_hits;
			}

		private:
			const Vec3f& a;
			const Vec3f& u;
			bool min_hits;
			Ci& min_c_world;
			float& min_distance;
			int& side;
		};

		bool PickCells(Cell* cells[], int n, const Vec3f& a, const Vec3f& u, Ci& min_c_world, float& min_distance, int& side)
		{
			CellPicking cp(a, u, min_c_world, min_distance, side);
			for(int i=0; i<n; i++) {
				cp(cells[i]);
			}
			return cp.hits();
		}

		bool PickCells(const std::vector<Cell*>& cells, const Vec3f& a, const Vec3f& u, Ci& min_c_world, float& min_distance, int& side)
		{
			CellPicking cp(a, u, min_c_world, min_distance, side);
			for(std::vector<Cell*>::const_iterator it=cells.begin(); it!=cells.end(); it++) {
				cp(*it);
			}
			return cp.hits();
		}

		bool PickCells(const std::map<Ci,Cell*>& cells, const Vec3f& a, const Vec3f& u, Ci& min_c_world, float& min_distance, int& side)
		{
			CellPicking cp(a, u, min_c_world, min_distance, side);
			for(std::map<Ci,Cell*>::const_iterator it=cells.begin(); it!=cells.end(); it++) {
				cp(it->second);
			}
			return cp.hits();
		}
	}

	/** Intersection by only checking direct "face" neighbour cells for intersection
	 * Warning: This method is NOT ACCURATE!
	 */
	bool CubesRayNear(Cubes* cubes, const Vec3f& a, const Vec3f& u, Ci& min_c_world, float& min_distance, int& side)
	{
		// in wich cell is the starting point
		Ci a_cube = Properties::PositionToWorld(a);
		Ci a_cell = Properties::WorldToCell(a_cube);
		std::vector<Cell*> cells;
		cells.reserve(4);
		Cell* cell;
		if(cubes->TryGetCell(a_cell, &cell)) {
			cells.push_back(cell);
		}
		if(cubes->TryGetCell(a_cell + Ci(Math::Sign(u.x()), 0, 0), &cell)) {
			cells.push_back(cell);
		}
		if(cubes->TryGetCell(a_cell + Ci(0, Math::Sign(u.y()), 0), &cell)) {
			cells.push_back(cell);
		}
		if(cubes->TryGetCell(a_cell + Ci(0, 0, Math::Sign(u.z())), &cell)) {
			cells.push_back(cell);
		}
		return Private::PickCells(cells, a, u, min_c_world, min_distance, side);
	}

	/** Intersection by checking all cells
	 * Warning: This method is SLOW!
	 */
	bool CubesRayAll(Cubes* cubes, const Vec3f& a, const Vec3f& u, Ci& min_c_world, float& min_distance, int& side)
	{
		return Private::PickCells(cubes->GetCells(), a, u, min_c_world, min_distance, side);
	}

	namespace Private
	{
		struct CubesWalker
		{
			CubesWalker(Cubes* cubes)
				: cubes_(cubes), last_cell_(0) {}

			bool operator()(const Ci& p) {
				Ci c_cell;
				Cu c_local;
				Properties::WorldToCellLocal(p, c_cell, c_local);
				if(last_cell_ == 0 || last_cell_->coordinate() != c_cell) {
					cubes_->TryGetCell(c_cell, &last_cell_);
				}
				if(last_cell_ != 0) {
					hits_ = IsSolid(last_cell_->At(c_local));
					return hits_;
				} else {
					hits_ = false;
					return true;
				}
			}

			/**
			 * @param move 0:x, 1:y, 2:z
			 * /
			bool operator()(const Ci& p, int move, int dir) {
				bool change_cell = false;
				if(move == 0) {
					if(dir < 0 && c_local_.x == 0) {
						c_local_.x = Properties::CellSize - 1;
						c_cell_.x --;
						change_cell = true;
					} else if(dir > 0 && c_local_.x == Properties::CellSize - 1) {
						c_local_.x = 0;
						c_cell_.x ++;
						change_cell = true;
					} else {
						local_data_ += dir;
					}
				} else if(move == 1) {
					if(dir < 0 && c_local_.y == 0) {
						c_local_.y = Properties::CellSize - 1;
						c_cell_.y --;
						change_cell = true;
					} else if(dir > 0 && c_local_.y == Properties::CellSize - 1) {
						c_local_.y = 0;
						c_cell_.y ++;
						change_cell = true;
					} else {
						local_data_ += dir * Properties::CellSize;
					}
				} else if(move == 2) {
					if(dir < 0 && c_local_.z == 0) {
						c_local_.z = Properties::CellSize - 1;
						c_cell_.z --;
						change_cell = true;
					} else if(dir > 0 && c_local_.z == Properties::CellSize - 1) {
						c_local_.z = 0;
						c_cell_.z ++;
						change_cell = true;
					} else {
						local_data_ += dir * Properties::CellSize * Properties::CellSize;
					}
				} else {
					// initialize
					Properties::WorldToCellLocal(p, c_cell_, c_local_);
					change_cell = true;
				}
				if(change_cell) {
					if(cubes_->TryGetCell(c_cell_, &last_cell_)) {
						is_empty_ = last_cell_->IsFullyEmpty();
						local_data_ = last_cell_->Begin() + Properties::LocalToIndex(c_local_);
					}
				}
				if(last_cell_ != 0) {
					if(is_empty_) {
						hits_ = false;
					} else {
						hits_ = IsSolid(CubeType(*local_data_));
					}
					return hits_;
				} else {
					hits_ = false;
					return true;
				}
			}*/

			bool hits() const { return hits_; }

		private:
			Cubes* cubes_;
			Ci c_cell_;
			Cu c_local_;
			const unsigned char* local_data_;
			bool is_empty_;
			Cell* last_cell_;
			bool hits_;
		};
	}

	/** Intersection by walking through the cubes in the direction of the ray
	 * Preffered and fastest method
	 */
	bool CubesRayCubeWalk(Cubes* cubes, const Vec3f& a, const Vec3f& u, Ci& result_cube, float& result_dist, int& result_side)
	{
		Private::CubesWalker cw(cubes);
		Collision::GridRayIntersect(a, u, cw, result_cube, result_dist, result_side);
		return cw.hits();
	}

	bool CubesRayCubeWalk(Cubes* cubes, const Vec3f& a, const Vec3f& u, float max_distance, float& result_dist, int& result_side)
	{
		// FIXME implement specialized faster method!
		Ci result_cube;
		bool hit = CubesRayCubeWalk(cubes, a, u, result_cube, result_dist, result_side);
		return hit && result_dist < max_distance;
	}

	bool CubesRayCubeWalk(Cubes* cubes, const Vec3f& a, const Vec3f& u, float max_distance, float& result_dist)
	{
		// FIXME implement specialized faster method!
		Ci result_cube;
		int result_side;
		bool hit = CubesRayCubeWalk(cubes, a, u, result_cube, result_dist, result_side);
		return hit && result_dist < max_distance;
	}

}
