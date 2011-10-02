#pragma once

#include "../Candy_Dll.h"
#include "Cube.h"
#include "Cell.h"
#include "CellIterator.h"
#include "Grid.h"
#include <Candy/Tools/LinAlg.h>
#include <Candy/Tools/Ptr.h>
#include <boost/function.hpp>
#include <boost/shared_array.hpp>
#include <boost/thread.hpp>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
//#ifndef NDEBUG
//	#define PRINT_NOTES
//#endif

class CANDY_API Cubes
{
public:
	Cubes()
	{}

	boost::function<void(Cell*)> OnAddCell;

	boost::function<void(Cell*, bool)> OnChangeCell;

	boost::function<void(Cell*, const CoordI&)> OnChangeCube;

	Cell* GetCell(const CoordI& c_cell) const;

	void SetType(const CoordI& cc_world, CubeType type) {
		CoordI c_cell = Common::WorldToCell(cc_world);
		Cell* cell = GetCell(c_cell);
		// set type in cell
		CoordU cc_local = Common::WorldToLocal(cc_world);
		cell->Set(cc_local, type);
		if(OnChangeCube) {
			OnChangeCube(cell, cc_world); // FIXME only call if changed
		}
	}

	CubeType GetType(const CoordI& cc_world) const {
		CoordI cc_cell = Common::WorldToCell(cc_world);
		Cell* cell;
		if(TryGetCell(cc_cell, &cell)) {
			// set type in cell
			CoordU cc_local = Common::WorldToLocal(cc_world);
			return cell->At(cc_local);
		} else {
			// cube does not exist
			return CubeTypes::NonExistent;
		}
	}

	CubeInteriorData* GetData(const CoordI& cc_world) const {
		CoordI cc_cell = Common::WorldToCell(cc_world);
		Cell* cell;
		if(TryGetCell(cc_cell, &cell)) {
			// set type in cell
			CoordU cc_local = Common::WorldToLocal(cc_world);
			return cell->AtData(cc_local);
		} else {
			// cube does not exist
			return 0;
		}
	}

	bool IsEmpty(const Vec3f& p) const {
		CoordI ccw = Common::PositionToInt(p);
		CubeInteriorData* d = GetData(ccw);
		return !d || IsEmptyOrNonExistent(d->GetType());
	}
	bool IsEmpty(float x, float y, float z) const {
		CoordI ccw = Common::PositionToInt(x, y, z);
		CubeInteriorData* d = GetData(ccw);
		return !d || IsEmptyOrNonExistent(d->GetType());
	}

	bool Exists(const CoordI& cc_world) const {
		CoordI cc_cell = Common::WorldToCell(cc_world);
		return ExistsCell(cc_cell);
	}

	bool HasData(const CoordI& cc_world, unsigned int side) const {
		return GetData(cc_world, side) != 0;
	}

	CubeSideData* GetData(const CoordI& cc_world, unsigned int side) const {
		// cell / border side index
		BorderSideId bs;
		Ci c_cell;
		Common::WorldToCellBorderSide(cc_world, side, c_cell, bs);
		Cell* cell = cells_[c_cell];
		if(cell == 0) {
			return 0;
		}
		return cell->border_sides_->Get(bs);
	}

	/*Cube Get(const CoordI& cc_world) {
		CoordI cc_cell = Common::WorldToCell(cc_world);
		Cell* cell;
		if(!TryGetCell(cc_cell, &cell)) {
			// cube does not exists
			return Cube(NonExistent);
		} else {
			CoordU cc_local = Common::WorldToLocal(cc_world);
			return Cube(
				cell->At(cc_local), // type
				_data.At(cc_world) // data or null if no data
			);
		}
	}*/

	size_t CountNonEmptyCells() const;

	size_t CountBorderQuads() const;

	size_t MemoryUsageBytes() const;

	float MemoryUsageMB() const;

	CellIterator IterateCells() const {
		return CellIterator(GetCells());
	}

	/** Executes an operation for all cells
	 * Op must provide (Cell* cell) -> void
	 */
	template<typename Op>
	void ApplyToAllCells(Op op) {
		for(CellIterator cit = IterateCells(); cit; ++cit) {
			op(cit.get());
		}
	}

	/** Executes an operation for all cubes in all cells
	 * Warning: This is probably resource intensive!
	 * Op must provide (CoordI cube_world_coordinate, CubeInteriorData* cube_data) -> void
	 */
	template<typename Op>
	void ApplyToAllCubes(Op op) {
		ApplyToAllCells([&](Cell* cell) {
				cell->ApplyToAllCubes(op);
		});
	}

	/** Computes intersection between a ray and all cubes */
	bool Pick(const Vec3f& a, const Vec3f& u, Ci& cube, float& min_distance, int& side);

	bool Pick(const Vec3f& a, const Vec3f& u, float max_distance, float& dist, int& side);

	bool Pick(const Vec3f& a, const Vec3f& u, float max_distance, float& dist);

	bool Pick(const Vec3f& a, const Vec3f& u, float max_distance);

	/** Adds CubeData for border cubes and deletes cube data for interior cubes */
	void VitalizeCubeData(Cell* cell);

	void VitalizeCube(const Cell::BorderSideIterator& it);

	/** Counts the number of border cubes for a cell */
	unsigned int CountBorderCubes(const Ci& c_cell) const;

	/** Creates mesh data of a cell */
	template<class Vertex, class Generator, class Data>
	void CreateMeshData(Cell* cell, Data/*Candy::MeshData<Vertex>*/& data) {
#ifdef PRINT_NOTES
		std::cout << "Meshing " << cell->coordinate() << std::endl;
#endif
		// write only vertices of visible sides. each side has its own vertices
		// a side is visible if one cube is empty and the other cube is solid
		data.vertices()->clear();
		//for(CellCubeIterator it=this->IterateCell(cell); it; ++it) {
		for(Cell::BorderSideIterator it=cell->IterateBorderSides(); it; ++it) {
			//// only create sides for border cubes
			//if(!it.IsBorder()) {
			//	continue;
			//}
			const Ci& cc_world = it.world();
			CubeType cube_type = it.type();
			CubeSideData* cube_data = it.data();
			unsigned int sideIndex = it.side();
			//// for each side of the cube
			//for(int sideIndex = 0; sideIndex < 6; sideIndex++) {
				// only add if the neighbour cube is empty
			//	if(this->IsBorderSide(cc_world, sideIndex)) {
					Generator::Add(data.vertices(), cc_world, cube_type, cube_data, sideIndex);
			//	}
			//}
		}
		// indices are trivial: (v) abcd -> (i) abcacd
		size_t quad_count = data.vertices()->size() / 4;
		std::vector<uint32_t>& indices = *data.indices();
		indices.resize(quad_count * 6);
		for(size_t i = 0; i < quad_count; ++i) {
			size_t p = 6 * i;
			uint32_t j = 4 * i;
			indices[p    ] = j;
			indices[p + 1] = j + 1;
			indices[p + 2] = j + 2;
			indices[p + 3] = j;
			indices[p + 4] = j + 2;
			indices[p + 5] = j + 3;
		}
	}

	/** Creates mesh data for a LOD version of the cubes
	 * Level of detail indicates:
	 *   0: normal mesh
	 *   1: merge 2x2x2 cubes into one
	 *   i: merge 2^i x 2^i x 2^i cubes into one
	 * @param lod level of detail
	 */
	template<class Vertex, class Generator, class Data>
	void CreateMeshDataLOD(Cell* cell, Data/*Candy::MeshData<Vertex>*/& data, int lod) {
		// FIXME implement
//		data.vertices()->clear();
//		// create lod mesh
//		unsigned int n = Common::CellSize - (1 << lod);
//		...
	}

public:
	CubeType SideNeighbourType(const CoordI& cc_world, int side) {
		// get coordinate of neighbour
		CoordI n = Geometry::GetSideNeighbour(cc_world, side);
		// get type
		return GetType(n);
	}

	bool IsBorderSide(const CoordI& cc_world, int side) {
		CubeType ct = SideNeighbourType(cc_world, side);
		return ::IsEmptyOrNonExistent(ct);
	}

	/** True if the cube is not empty and at least one of its neighbours is empty */
	bool IsBorderCube(const CoordI& cc_world) {
		return !::IsEmpty(GetType(cc_world))
			&& (
				IsBorderSide(cc_world, 0)
				|| IsBorderSide(cc_world, 1)
				|| IsBorderSide(cc_world, 2)
				|| IsBorderSide(cc_world, 3)
				|| IsBorderSide(cc_world, 4)
				|| IsBorderSide(cc_world, 5)
			);
	}

	/** Gets a list with all cells */
	std::vector<Cell*> GetCells() const;

	bool ExistsCell(const CoordI& cc_cell) const {
		return cells_.valid(cc_cell);
	}

	bool TryGetCell(const CoordI& cc_cell, Cell** cell_ptr) const {
		*cell_ptr = cells_[cc_cell];
		return *cell_ptr != 0;
	}

private:
	bool PickGridWalk(const Vec3f& a, const Vec3f& u, Ci& min_c_world, float& min_distance, int& side);
	bool PickNear(const Vec3f& a, const Vec3f& u, Ci& min_c_world, float& min_distance, int& side);

private:
	mutable Grid<Cell*> cells_;
};
