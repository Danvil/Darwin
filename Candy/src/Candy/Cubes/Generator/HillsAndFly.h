/*
 * HillsAndFly.h
 *
 *  Created on: Sep 23, 2011
 *      Author: david
 */

#ifndef HILLSANDFLY_
#define HILLSANDFLY_

#include <Candy/Tools/Perlin.h>

namespace Generators
{
	struct HillsAndFly
	{
		CubeType operator()(const CoordI& cw) {
			// phase 1 : hills
			{
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
			}
			// phase 2 : fly
			{
				const double cScale = 0.3;
				float px = Common::WorldToPositionCenter(cw.x);
				float py = Common::WorldToPositionCenter(cw.y);
				float pz = Common::WorldToPositionCenter(cw.z);
				double n = Perlin::NoiseMP(cScale * px, cScale * py, cScale * pz);
				if(n > 0.5) {
					return CubeTypes::Mud;
				}
			}
			// its air
			return CubeTypes::Empty;
		}
	};
}

#endif /* PERLINHILLS_H_ */
