#pragma once

#include "../Lighting.hpp"
#include "../Cube.h"
#include "../Cell.h"
#include <Candy/Tools/Coord.h>

class Cubes;

namespace Hexa
{
namespace Lighting
{
	/** Direct fast lighting computation with no indirect lighting */
	class DirectLighting
	: public GlobalIlluminator
	{
	public:
		static const unsigned int cLightingSamplesValue = 1;
		uint64_t Iterate(Ptr(Cubes) cubes);
		static void ComputeCell(Cubes* cubes, Cell* cell);
		static void ComputeCube(Cubes* cubes, const Cell::BorderSideIterator& it);
		static void ComputeCube(Cubes* cubes, const CoordI& cw, unsigned int side, CubeType type, CubeSideData* data);
	};

}}
