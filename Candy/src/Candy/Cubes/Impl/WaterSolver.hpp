/*
 * WaterSolver.hpp
 *
 *  Created on: 09.08.2011
 *      Author: david
 */

#ifndef WATERSOLVER_HPP_
#define WATERSOLVER_HPP_

#include "../Cubes.h"

namespace CubesImpl
{
	namespace WaterSolver
	{
		inline float& Water(CubeInteriorData* cd) {
			return cd->water;
		}

		void Solve(Ptr(Cubes) cubes, float dt);
	}

}

#endif /* WATERSOLVER_HPP_ */
