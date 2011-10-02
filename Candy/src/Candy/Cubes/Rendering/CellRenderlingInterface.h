#pragma once

#include "../Cell.h"

class CellRenderlingInterface
{
public:
	virtual ~CellRenderlingInterface() {}

	virtual void Render() = 0;

	virtual void Invalidate() = 0;

	virtual void UpdateMesh() = 0;

	virtual Cell* GetCell() const = 0;

};

