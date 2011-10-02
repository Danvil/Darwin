/*
 * WaterSolver.cpp
 *
 *  Created on: 09.08.2011
 *      Author: david
 */


#include "WaterSolver.hpp"
#include <cmath>

namespace CubesImpl
{
	namespace WaterSolver
	{
		float interpolate(float r, float a, float b) {
			return (1.0f - r)*a + r*b;
		}

		/** Linearly interpolates the water value at the given position
		 * f provides float(int,int,int)
		 */
		template<typename F>
		float LinearlyInterpolatedValue(const Vec3f& p, F f) {
			// position in the
			float xif, yif, zif; // integral values
			float xr, yr, zr; // remainder values
			xr = std::modf(p.x() - 0.5f, &xif);
			yr = std::modf(p.y() - 0.5f, &yif);
			zr = std::modf(p.z() - 0.5f, &zif);
			int x = int(xif);
			int y = int(yif);
			int z = int(zif);
			// get values from the 8 contributing cubes
			float vals[2][2][2] = {
					{
							{f(x, y, z), f(x, y, z+1)},
							{f(x, y+1, z), f(x, y+1, z+1)}
					},
					{
							{f(x+1, y, z), f(x+1, y, z+1)},
							{f(x+1, y+1, z), f(x+1, y+1, z+1)}
					}
			};
			// interpolate
			float a11 = interpolate(zr, vals[0][0][0], vals[0][0][1]);
			float a12 = interpolate(zr, vals[0][1][0], vals[0][1][1]);
			float a21 = interpolate(zr, vals[1][0][0], vals[1][0][1]);
			float a22 = interpolate(zr, vals[1][1][0], vals[1][1][1]);
			float b1 = interpolate(yr, a11, a12);
			float b2 = interpolate(yr, a21, a22);
			float c = interpolate(zr, b1, b2);
			return c;
		}

		void advect(Ptr(Cubes) cubes, float dt, const CoordI& cw, CubeInteriorData* cd) {
			Vec3f p = Common::WorldToPositionCenter(cw);
			// look back where it flowed from
			p.x() -= dt * cd->water_vel_x;
			p.y() -= dt * cd->water_vel_y;
			p.z() -= dt * cd->water_vel_z;
			// get water value there
			float water = LinearlyInterpolatedValue(p, [&](int x, int y, int z) {
				CubeInteriorData* data = cubes->GetData(CoordI(x,y,z));
				return data ? data->water : 0.0f;
			});
			// FIXME do something ...
		}

		void diffuse(float dt, const CoordI& cw, CubeInteriorData* cd) {

		}

		void Prepare(Ptr(Cubes) cubes) {
			cubes->ApplyToAllCubes([&](const CoordI& cw, CubeInteriorData* cd) {
				cd->water = 0.0f;
				cd->water_vel_x = 0.0f;
				cd->water_vel_y = 0.0f;
				cd->water_vel_z = 0.0f;
			});
		}

		void Solve(Ptr(Cubes) cubes, float dt) {
			// advect
			cubes->ApplyToAllCubes([&](const CoordI& cw, CubeInteriorData* cd) {
				advect(cubes, dt, cw, cd);
			});
		}

	}
}
