#include "CubesRenderling.h"
#include "CellRenderlingFactory.h"


CellRenderling::CellRenderling(Cubes* cubes, Cell* cell)
{
	underlying = CellRenderlingFactory::FactorPLT(cubes, cell);
}

CellRenderling::~CellRenderling()
{
}

void CellRenderling::Render()
{
	underlying->Render();
}

void CellRenderling::Invalidate()
{
	underlying->Invalidate();
}

void CellRenderling::UpdateMesh()
{
	underlying->UpdateMesh();
}


CubesRenderling::CubesRenderling(Cubes* cubes)
: _cubes(cubes)
{
}

CubesRenderling::~CubesRenderling()
{
}

void CubesRenderling::Render()
{
	guard_.lock();
	for(CellRenderlingContainer::const_iterator it=_cell_renderlings.begin(); it!=_cell_renderlings.end(); ++it) {
		if(IsVisible(it->first)) {
			it->second->Render();
		}
	}
	guard_.unlock();
}

void CubesRenderling::UpdateMeshAll()
{
	guard_.lock();
	for(CellRenderlingContainer::const_iterator it=_cell_renderlings.begin(); it!=_cell_renderlings.end(); ++it) {
		it->second->UpdateMesh();
	}
	guard_.unlock();
}

void CubesRenderling::NotifyAddCell(Cell* cell)
{
	guard_.lock();
	_cell_renderlings[cell->coordinate()] = new CellRenderling(_cubes, cell);
	guard_.unlock();
}

void CubesRenderling::NotifyInvalidate(Cell* cell)
{
	guard_.lock();
	_cell_renderlings[cell->coordinate()]->Invalidate();
	guard_.unlock();
}

bool CubesRenderling::IsVisible(const Ci& c_cell)
{
	const float cRadius = float(Common::CellSize);
	if(IDrawable::sCamera->DistanceToEye(Common::CellMid(c_cell)) < cRadius) {
		return true;
	}
	for(int i=0; i<8; i++) {
		Vec3f p = Geometry::CellCorner(c_cell, i);
		if(IDrawable::sCamera->IsVisibleByCamera(p)) {
			return true;
		}
	}
	return false;
}
