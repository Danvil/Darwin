#pragma once

#include "../Cell.h"

class CellRenderlingInterface
{
public:
	virtual ~CellRenderlingInterface() {}

	virtual void Render() = 0;

	virtual bool UpdateMesh() = 0;

	virtual Cell* GetCell() const = 0;

};

