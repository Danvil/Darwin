#include "CellRenderlingFactory.h"
//#include "CellRenderlings/Pnc.h"
//#include "CellRenderlings/Pl.h"
#include "CellRenderlings/Plt.h"

CellRenderlingInterface* CellRenderlingFactory::FactorPNC(Cubes* cubes, Cell* cell)
{
	return 0;//new CellRenderlings::PNC::PncRenderling(cubes, cell);
}

CellRenderlingInterface* CellRenderlingFactory::FactorPL(Cubes* cubes, Cell* cell)
{
	return 0;//new CellRenderlings::PL::PlRenderling(cubes, cell);
}

CellRenderlingInterface* CellRenderlingFactory::FactorPLT(Cubes* cubes, Cell* cell)
{
	return new CellRenderlings::PLT::Renderling(cubes, cell);
}
