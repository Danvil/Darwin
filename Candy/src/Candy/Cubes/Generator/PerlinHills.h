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
			float px = Common::WorldToPositionCenter(cw.x);
			float py = Common::WorldToPositionCenter(cw.y);
			float pz = Common::WorldToPositionCenter(cw.z);
			double n = cAmplitude * (cOffset + Perlin::Noise(cScale * px, cScale * py));
			if(pz < n) {
				return CubeTypes::Mud;
			}
			else {
				return CubeTypes::Empty;
			}
		}
	};
}

#endif /* PERLINHILLS_H_ */
