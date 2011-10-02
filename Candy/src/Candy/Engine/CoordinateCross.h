#pragma once

#include "IDrawable.h"

class CoordinateCross
	: public Candy::IDrawable
{
public:
	CoordinateCross();
	~CoordinateCross();
	void Render();
};

