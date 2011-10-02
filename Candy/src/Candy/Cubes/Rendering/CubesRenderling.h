#pragma once

#include "../../Candy_Dll.h"
#include "../Cubes.h"
#include "CellRenderlingInterface.h"
#include <Candy/Engine/IDrawable.h>
#include <map>
#include <boost/thread.hpp>

class CANDY_API CellRenderling
: public Candy::IDrawable
{
public:
	CellRenderling(Cubes* cubes, Cell* cell);

	~CellRenderling();

	void Render();

	void Invalidate();

	void UpdateMesh();

private:
	CellRenderlingInterface* underlying;

};

class CANDY_API CubesRenderling
: public Candy::IDrawable
{
public:
	CubesRenderling(Cubes* cubes);

	~CubesRenderling();

	void Render();

	void UpdateMeshAll();

	void NotifyAddCell(Cell* cell);

	void NotifyInvalidate(Cell* cell);

private:
	static bool IsVisible(const Ci& c_cell);

private:
	Cubes* _cubes;
	typedef std::map<CoordI, CellRenderling*> CellRenderlingContainer;
	CellRenderlingContainer _cell_renderlings;
	boost::mutex guard_;
};

