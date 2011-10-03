/*
 * SphericalTerra.h
 *
 *  Created on: Oct 3, 2011
 *      Author: david
 */

#ifndef GENERATOR_SPHERICALTERRA_H_
#define GENERATOR_SPHERICALTERRA_H_

#include <libnoise/noise.h>

namespace Generators
{
	struct SphericalTerra
	{
		CubeType operator()(const CoordI& cw)
		{
			static noise::module::Perlin myModule;
			static noise::model::Sphere sphere_model(myModule);

			double lat, lon;
			Properties::WorldToAnglesDegrees(cw, lon, lat);
			double x = 15.0 * sphere_model.GetValue(lat, lon - Math::Pi<double>());

			bool is_solid = (double(cw.z) < x);

			if(is_solid) {
				return CubeTypes::Mud;
			}
			else {
				return CubeTypes::Empty;
			}
		}
	};
}

#endif
