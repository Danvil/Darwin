#pragma once

#include "CellRenderlingInterface.h"
#include <Candy/Candy_Dll.h>
#include <Candy/Cubes/Cubes.h>
#include <Candy/Engine/IDrawable.h>
#include <map>
#include <boost/thread.hpp>

class CANDY_API CubesRenderling
: public Candy::IDrawable
{
public:
	CubesRenderling(Cubes* cubes);

	virtual ~CubesRenderling();

	void Render();

	void UpdateMeshAll();

	void NotifyAddCell(Cell* cell);

private:
	static bool IsVisible(const Ci& c_cell);

private:
	Cubes* cubes_;
	typedef std::map<CoordI, CellRenderlingInterface*> CellRenderlingContainer;
	CellRenderlingContainer cell_renderlings_;
	boost::mutex guard_;
};

