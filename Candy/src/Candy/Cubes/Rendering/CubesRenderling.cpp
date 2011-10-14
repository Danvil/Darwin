#include "CubesRenderling.h"
#include "CellRenderling.h"

CubesRenderling::CubesRenderling(Cubes* cubes)
: cubes_(cubes)
{
}

CubesRenderling::~CubesRenderling()
{
}

void CubesRenderling::Render()
{
	guard_.lock();
	for(CellRenderlingContainer::const_iterator it=cell_renderlings_.begin(); it!=cell_renderlings_.end(); ++it) {
		if(IsVisible(it->first)) {
			it->second->Render();
		}
	}
	guard_.unlock();
}

size_t CubesRenderling::UpdateMeshAll()
{
	size_t n = 0;
	guard_.lock();
	for(CellRenderlingContainer::const_iterator it=cell_renderlings_.begin(); it!=cell_renderlings_.end(); ++it) {
		if(it->second->UpdateMesh()) {
			n++;
		}
	}
	guard_.unlock();
	return n;
}

void CubesRenderling::NotifyAddCell(Cell* cell)
{
	guard_.lock();
	cell_renderlings_[cell->coordinate()] = new CandyCubes::Rendering::CellRenderling(cubes_, cell);
	guard_.unlock();
}

bool CubesRenderling::IsVisible(const Ci& c_cell)
{
	const float cRadius = float(Properties::CellSize);
	if(IDrawable::sCamera->DistanceToEye(Properties::CellMid(c_cell)) < cRadius) {
		return true;
	}
	for(int i=0; i<8; i++) {
		Vec3f p = Properties::CellCorner(c_cell, i);
		if(IDrawable::sCamera->IsVisibleByCamera(p)) {
			return true;
		}
	}
	return false;
}
