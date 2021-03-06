/*
 * PerlinHills.h
 *
 *  Created on: May 2, 2011
 *      Author: david
 */

#ifndef PERLINHILLS_H_
#define PERLINHILLS_H_

#include <Candy/Tools/Perlin.h>

namespace Generators
{
	struct PerlinHills
	{
		CubeType operator()(const CoordI& cw) {
			const double cScale = 0.03;
			const double cAmplitude = 8.0f;
			const double cOffset = 1.0f;

//			Vec3f p = Properties::WorldToPositionCenter(cw);
//			double n = cAmplitude * (cOffset + Perlin::Noise(cScale * p[0], cScale * p[1]));
//			bool is_solid = (p[2] < n);

			double n = cAmplitude * (cOffset + Perlin::Noise(cScale * float(cw.x), cScale * float(cw.y)));
			bool is_solid = (float(cw.z) < n);

			if(is_solid) {
				return CubeTypes::Mud;
			}
			else {
				return CubeTypes::Empty;
			}
		}
	};
}

#endif /* PERLINHILLS_H_ */
