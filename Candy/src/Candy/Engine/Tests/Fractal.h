#pragma once

#include "../Node.h"

namespace Candy
{
	namespace Fractal
	{
		/** Creates an array of cubes for testing */
		Ptr(Candy::Node) ConstructCrateArray();

		/** Creates a sierpinsik construct of cubes for testing */
		Ptr(Candy::Node) ConstructSierpinskiCubes(unsigned int depth);

		/** Creates a sierpinsik construct of tetraeder for testing */
		Ptr(Candy::Node) ConstructSierpinski(unsigned int depth);

	}
}
