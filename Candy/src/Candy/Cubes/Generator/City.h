/*
 * City.h
 *
 *  Created on: May 20, 2011
 *      Author: david
 */

#ifndef CANDY_CUBES_GENERATOR_CITY_H_
#define CANDY_CUBES_GENERATOR_CITY_H_

#include <Candy/Tools/Perlin.h>

namespace Generators
{
	struct City
	{
		CubeType operator()(const CoordI& cw) {
			const unsigned int lamp1 = 10;
			const unsigned int lamp2 = 13;
			const unsigned int lamp_h = 3;
			const unsigned int house = 8;
			const unsigned int house_h = 6;
			CoordU cl = Common::WorldToLocal(cw);
			// ground
			if(cw.z < 0) {
				return CubeTypes::Mud;
			}
			// house body
			if(cl.x < house && cl.y < house && cw.z < int(house_h)) {
				return CubeTypes::Stone;
			}
			// street lamp
			if((cl.x == lamp1 || cl.x == lamp2)
			&& (cl.y == lamp1 || cl.y == lamp2)) {
				if(cw.z < int(lamp_h)) {
					return CubeTypes::Indestructible;
				}
				else if(cw.z == int(lamp_h)) {
					if(cl.x == lamp1 && cl.y == lamp1) {
						return CubeTypes::LightWhite;
					}
					else if(cl.x == lamp1 && cl.y == lamp2) {
						return CubeTypes::LightRed;
					}
					else if(cl.x == lamp2 && cl.y == lamp1) {
						return CubeTypes::LightGreen;
					}
					else if(cl.x == lamp2 && cl.y == lamp2) {
						return CubeTypes::LightBlue;
					}
				}
			}
			return CubeTypes::Empty;
		}
	};
}

#endif
