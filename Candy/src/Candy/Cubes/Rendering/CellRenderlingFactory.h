#pragma once

#include "../../Candy_Dll.h"
#include "CellRenderlingInterface.h"
#include "../Cubes.h"

namespace CellRenderlingFactory
{
	CANDY_API CellRenderlingInterface* FactorPNC(Cubes* cubes, Cell* cell);

	CANDY_API CellRenderlingInterface* FactorPL(Cubes* cubes, Cell* cell);

	CANDY_API CellRenderlingInterface* FactorPLT(Cubes* cubes, Cell* cell);

}
