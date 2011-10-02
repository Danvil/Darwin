/*
 * LightingHelpers.hpp
 *
 *  Created on: 11.08.2011
 *      Author: david
 */

#ifndef LIGHTINGHELPERS_HPP_
#define LIGHTINGHELPERS_HPP_

#include "../Common.h"
#include <Candy/Tools/Random.h>
#include <cassert>

namespace Hexa
{
	namespace Lighting
	{

		/*template<typename T>
		void IterativeMean(T& a, unsigned int na, const T& b, unsigned int nb) {
			float scl2 = 1.0f / float(na + nb);
			float scl1 = float(na) * scl2;
			a = scl1 * a + scl2 * b;
		}

		inline void IterativeMean(float& a, unsigned int na, float b, unsigned int nb) {
			a = (float(na) * a + b) / float(na + nb);
		}*/

		/** Gets the cube face center point but slightly outside the cube */
		inline Vec3f CenterPointSlightlyOutsideOfCubeSide(const CoordI& c_world, unsigned int sideIndex) {
			return Properties::CubeSidePoint(c_world, sideIndex, 0.51f, 0.0f, 0.0f);
		}

		const unsigned int PatternCubeSidePointCount = 9;

		inline Vec3f PatternCubeSidePoint(const CoordI& c_world, unsigned int sideIndex, unsigned int i) {
			const float LightSampleGrid[PatternCubeSidePointCount][2] = {
					{0, 0}, // center
					{+0.25f, 0},
					{-0.25f, 0},
					{0, +0.25f},
					{0, -0.25f},
					{+0.25f, +0.25f},
					{+0.25f, -0.25f},
					{-0.25f, +0.25f},
					{-0.25f, -0.25f}
			};
			assert(i < PatternCubeSidePointCount);
			float u = LightSampleGrid[i][0];
			float v = LightSampleGrid[i][1];
			return Properties::CubeSidePoint(c_world, sideIndex, 0.51f, u, v);
		}

		/** Gets a random point on the cube face but slightly outside the cube */
		inline Vec3f RandomCubeSidePoint(const CoordI& c_world, unsigned int sideIndex) {
			float h = 0.51f;
			float u = 0.49f * Random::UniformMPf();
			float v = 0.49f * Random::UniformMPf();
			return Properties::CubeSidePoint(c_world, sideIndex, h, u, v);
		}

		inline void QuadMidAndNormal(const CoordI& c_world, unsigned int sideIndex, Vec3f& mid, Vec3f& normal) {
			const float h = 0.51f;
			mid = Properties::CubeSidePoint(c_world, sideIndex, h, 0.0f, 0.0f);
			normal = Properties::CubeSidePoint(c_world, sideIndex, 1.0f, 0.0f, 0.0f);
		}

		/// <summary>
		/// Returns a uniform distributed direction vector which points away from the given cube side
		/// </summary>
		/// <param name="sideIndex"></param>
		/// <returns></returns>
		inline Vec3f RandomDirectionFromCubeSide(const CoordI& c_world, int sideIndex, float& n_dot_u) {
			float a, b, t;
			Random::UniformOnHalfSphere(a, b, t);
			n_dot_u = t;
			return Properties::CubeSidePoint(c_world, sideIndex, t, a, b);
			//Random::PseudoUniformOnHalfSphere(a, b, t);
		}
	}
}

#endif /* LIGHTINGHELPERS_HPP_ */
