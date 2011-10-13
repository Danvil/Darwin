#pragma once

#include "../../Candy_Dll.h"
#include "../Generator.h"
#include "../Lighting.hpp"
#include <Candy/Cubes/Rendering/CubesRenderling.h>
#include <boost/thread.hpp>

class Cubes;
class GroundHeightLookup;
struct Cell;

class CANDY_API Background
{
public:
	Background(Ptr(Cubes) cubes, Ptr(CubesRenderling) osgman, Ptr(GroundHeightLookup) height, Ptr(Generator) generator);
	~Background();

	void enableBuildFirst() {
		build_first_ = true;
	}

	void Start();

	void Stop();

	void PrintStatus();

private:
	void Run();

	void UpdateLighting(Cell* cell, unsigned int samples_count);
	
	Ptr(Cubes) cubes_;
	Ptr(CubesRenderling) osgman_;
	Ptr(GroundHeightLookup) height_lookup_;
	Ptr(Generator) generator_;
	Ptr(Hexa::GlobalIlluminator) gi_basic_;
	Ptr(Hexa::GlobalIlluminator) gi_;

	bool build_first_;

	bool _running;
	boost::thread _thread;
};

