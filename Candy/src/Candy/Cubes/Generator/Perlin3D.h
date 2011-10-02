/*
 * Perlin3D.h
 *
 *  Created on: May 20, 2011
 *      Author: david
 */

#ifndef CANDY_CUBES_GENERATOR_PERLIN3D_H_
#define CANDY_CUBES_GENERATOR_PERLIN3D_H_

#include <Candy/Tools/Perlin.h>

namespace Generators
{
	struct Perlin3D
	{
		CubeType operator()(const CoordI& cw) {
			const double cScale = 0.03;
			float px, py, pz;
			Properties::WorldToPositionCenter(cw, px, py, pz);
			double n = Perlin::NoiseMP(cScale * px, cScale * py, cScale * pz);
			if(n > 0) {
				return CubeTypes::Mud;
			}
			else {
				return CubeTypes::Empty;
			}
		}
	};
}

#endif
