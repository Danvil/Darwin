/*
 * ZeroHeight.h
 *
 *  Created on: Oct 3, 2011
 *      Author: david
 */

#ifndef GENERATORS_ZEROHEIGHT_H_
#define GENERATORS_ZEROHEIGHT_H_

#include <Candy/Tools/Perlin.h>

namespace Generators
{
	struct ZeroHeight
	{
		CubeType operator()(const CoordI& cw) {
			if(float(cw.z) < 0) {
				return CubeTypes::Mud;
			}
			else {
				return CubeTypes::Empty;
			}
		}
	};
}

#endif
