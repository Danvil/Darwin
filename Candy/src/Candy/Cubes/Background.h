#pragma once

#include "../Candy_Dll.h"
#include "Lighting.hpp"
#include <Candy/Cubes/Rendering/CubesRenderling.h>
#include <boost/thread.hpp>

class Cubes;
struct Cell;

class CANDY_API Background
{
public:
	Background(Ptr(Cubes) cubes, Ptr(CubesRenderling) osgman);
	~Background();

	void Start();

	void Stop();

	void PrintStatus();

private:
	void Run();

	void UpdateLighting(Cell* cell, unsigned int samples_count);
	
	Ptr(Cubes) cubes_;
	Ptr(CubesRenderling) osgman_;
	Ptr(Hexa::GlobalIlluminator) gi_basic_;
	Ptr(Hexa::GlobalIlluminator) gi_;

	bool _running;
	boost::thread _thread;
};

