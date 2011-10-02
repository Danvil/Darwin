#include "Cubes.h"
#include "Collision/CubesRay.h"
#include "Impl/DirectLighting.h"
#include "Appearance.h"
#include <list>
#include <cassert>

Cell* Cubes::GetCell(const CoordI& c_cell) const {
	Cell* cell;
	if(!TryGetCell(c_cell, &cell)) {
		// create cell
		cell = new Cell(c_cell);
		cells_.set(c_cell, cell);
		if(OnAddCell) {
			OnAddCell(cell);
		}
	}
	return cell;
}

size_t Cubes::CountNonEmptyCells() const
{
	return cells_.valid_cells().size();
}

size_t Cubes::CountBorderQuads() const
{
	size_t cnt = 0;
	const std::vector<Cell*>& active_cells = cells_.valid_cells();
	std::for_each(active_cells.begin(), active_cells.end(), [&](Cell* cell) {
		cnt += size_t(cell->border_sides_->Count());
	});
	return cnt;
}

size_t Cubes::MemoryUsageBytes() const
{
	return
			// cubes
			CountNonEmptyCells() * size_t(Common::CellCubeCount) * sizeof(CubeInteriorData)
			// faces
			+ CountBorderQuads() * sizeof(CubeSideData);
}

float Cubes::MemoryUsageMB() const
{
	return float(MemoryUsageBytes()) / 1024.0f / 1024.0f;
}

//float Cubes::GetGroundZ(float px, float py, float pz) {
//	Ci cw = Common::PositionToInt(Vec3f(px, py, pz));
//	CubeType initial_type = GetType(cw);
//	if(initial_type == CubeTypes::NonExistent) {
//		return pz;
//	}
//	bool initial_empty = IsEmpty(initial_type);
//	int d = initial_empty ? -1 : +1;
//	while(true) {
//		cw.z += d;
//		CubeType curr_type = GetType(cw);
//		if(IsEmpty(curr_type) != initial_empty || curr_type == CubeTypes::NonExistent) {
//			return Common::WorldToPosition(cw.z) + 0.5f*(1.0f - float(d)) * Common::CubeSize;
//		}
//	}
//}
//
//float Cubes::GetGroundZ_NonExistentFromGround(float px, float py, float pz) {
//	Ci cw = Common::PositionToInt(Vec3f(px, py, pz));
//	CubeType initial_type = GetType(cw);
//	bool initial_empty = IsEmpty_NonExistentGround(initial_type, pz > 0);
//	int d = initial_empty ? -1 : +1;
//	while(true) {
//		cw.z += d;
//		CubeType curr_type = GetType(cw);
//		if(IsEmpty_NonExistentGround(curr_type, cw.z > 0) != initial_empty) {
//			return Common::WorldToPosition(cw.z) + 0.5f*(1.0f - float(d)) * Common::CubeSize;
//		}
//	}
//}

bool Cubes::Pick(const Vec3f& a, const Vec3f& u, Ci& min_c_world, float& min_distance, int& side)
{
	return Collision::CubesRayCubeWalk(this, a, u, min_c_world, min_distance, side);
}

bool Cubes::Pick(const Vec3f& a, const Vec3f& u, float max_distance, float& dist, int& side)
{
	return Collision::CubesRayCubeWalk(this, a, u, max_distance, dist, side);
}

bool Cubes::Pick(const Vec3f& a, const Vec3f& u, float max_distance, float& dist)
{
	return Collision::CubesRayCubeWalk(this, a, u, max_distance, dist);
}

bool Cubes::Pick(const Vec3f& a, const Vec3f& u, float max_distance)
{
	float dist; // TODO do not care
	return Collision::CubesRayCubeWalk(this, a, u, max_distance, dist);
}

namespace Impl
{
	inline void vitalize_cube_no_lighting(Cubes*, const Cell::BorderSideIterator& it) {
		CubeSideData* x = it.data();
		x->object_color = Appearance::CubeBaseColor(it.type());
	}

	inline void vitalize_cube_with_lighting(Cubes* cubes, const Cell::BorderSideIterator& it) {
		vitalize_cube_no_lighting(cubes, it);
		// use fast direct lighting algorithm to compute lighting
		Hexa::Lighting::DirectLighting::ComputeCube(cubes, it);
	}
}

void Cubes::VitalizeCubeData(Cell* cell)
{
	if(cell->IsFullyEmpty()) {
		cell->border_sides_->Purge(); // free all memory for this cell
		return;
	}
#ifdef PRINT_NOTES
	std::cout << "Vitalizing " << cell->coordinate() << std::endl;
#endif
	// create new border side data
	std::vector<BorderSideId> border_sides_list;
	border_sides_list.reserve(Common::CellCubeCount); // TODO better estimate?
	for(Cell::CubeIterator it=cell->IterateCubes(); it.valid(); it.next()) {
		CubeType type = it.type();
		if(::IsEmpty(type)) {
			continue;
		}
		const Ci& cc_world = it.world();
		const Cu& cc_local = it.local();
		for(unsigned int i=0; i<6; i++) {
			if(IsBorderSide(cc_world, i)) {
				// add to border sides
				BorderSideId bs = Common::BuildBorderSide(cc_local, i);
				border_sides_list.push_back(bs);
			}
		}
	}
	// save old lighting
	LinearKeyValueContainer* old_side_data = cell->border_sides_;
	// change border side data container
	cell->border_sides_ = new LinearKeyValueContainer();
	cell->border_sides_->Add(border_sides_list.begin(), border_sides_list.end());
	// copy from old data if there was some
	bool first_vitalize = (old_side_data != 0);
	if(first_vitalize) {
		for(Cell::BorderSideIterator jt=cell->IterateBorderSides(); jt; ++jt) {
			BorderSideId id = jt.id();
			if(old_side_data->HasKey(id)) {
				*(jt.data()) = *(old_side_data->Get(id));
			}
			else {
				Impl::vitalize_cube_with_lighting(this, jt);
			}
		}
	}
	else {
		// set cube side base color
		for(Cell::BorderSideIterator jt=cell->IterateBorderSides(); jt; ++jt) {
			Impl::vitalize_cube_no_lighting(this, jt);
		}
	}
	// delete old
	delete old_side_data;
	// reset light samples
	cell->_lighting_samples = 0;
	// notify that cell data has changed
	cell->is_dirty_ = false;
	if(OnChangeCell) {
		OnChangeCell(cell, first_vitalize);
	}
}

void Cubes::VitalizeCube(const Cell::BorderSideIterator& it, bool compute_lighting)
{
	if(compute_lighting) {
		Impl::vitalize_cube_with_lighting(this, it);
	}
	else {
		Impl::vitalize_cube_no_lighting(this, it);
	}
}

unsigned int Cubes::CountBorderCubes(const Ci& c_cell) const
{
	Cell* cell = cells_[c_cell];
	return cell ? cell->border_sides_->Count() : 0;
}

std::vector<Cell*> Cubes::GetCells() const
{
	return cells_.valid_cells();
}

