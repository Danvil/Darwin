/*
 * LightingHelpers.hpp
 *
 *  Created on: 11.08.2011
 *      Author: david
 */

#ifndef LIGHTINGHELPERS_HPP_
#define LIGHTINGHELPERS_HPP_

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

		inline Vec3f CubeSidePoint(unsigned int sideIndex, float h, float u, float v) {
			switch(sideIndex) {
				case 0: return Vec3f(u, v, -h);
				case 1: return Vec3f(-h, u, v);
				case 2: return Vec3f(u, +h, v);
				case 3: return Vec3f(+h, u, v);
				case 4: return Vec3f(u, -h, v);
				case 5: return Vec3f(u, v, +h);
				default: assert(false);
			}
		}

		/** Gets the cube face center point but slightly outside the cube */
		inline Vec3f CenterPointSlightlyOutsideOfCubeSide(unsigned int sideIndex) {
			return CubeSidePoint(sideIndex, 0.51f, 0.0f, 0.0f);
		}

		const unsigned int PatternCubeSidePointCount = 9;

		inline Vec3f PatternCubeSidePoint(unsigned int sideIndex, unsigned int i) {
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
			return CubeSidePoint(sideIndex, 0.51f, u, v);
		}

		/** Gets a random point on the cube face but slightly outside the cube */
		inline Vec3f RandomCubeSidePoint(unsigned int sideIndex) {
			float h = 0.51f;
			float u = 0.49f * Random::UniformMPf();
			float v = 0.49f * Random::UniformMPf();
			return CubeSidePoint(sideIndex, h, u, v);
		}

		inline void QuadMidAndNormal(unsigned int sideIndex, Vec3f& mid, Vec3f& normal) {
			const float h = 0.51f;
			switch(sideIndex) {
			case 0:
				mid = Vec3f(0, 0, -h);
				normal = Vec3f(0, 0, -1);
				break;
			case 1:
				mid = Vec3f(-h, 0, 0);
				normal = Vec3f(-1, 0, 0);
				break;
			case 2:
				mid = Vec3f(0, +h, 0);
				normal = Vec3f(0, +1, 0);
				break;
			case 3:
				mid = Vec3f(+h, 0, 0);
				normal = Vec3f(+1, 0, 0);
				break;
			case 4:
				mid = Vec3f(0, -h, 0);
				normal = Vec3f(0, -1, 0);
				break;
			case 5:
				mid = Vec3f(0, 0, +h);
				normal = Vec3f(0, 0, +1);
				break;
			}
		}

		/// <summary>
		/// Returns a uniform distributed direction vector which points away from the given cube side
		/// </summary>
		/// <param name="sideIndex"></param>
		/// <returns></returns>
		inline Vec3f RandomDirectionFromCubeSide(int sideIndex, float& n_dot_u) {
			float a, b, t;
			Random::UniformOnHalfSphere(a, b, t);
			n_dot_u = t;
			//Random::PseudoUniformOnHalfSphere(a, b, t);
			// create point in the correct half sphere
			switch(sideIndex) {
				case 0: return Vec3f(a, b, -t);
				case 1: return Vec3f(-t, a, b);
				case 2: return Vec3f(a, +t, b);
				case 3: return Vec3f(+t, a, b);
				case 4: return Vec3f(a, -t, b);
				case 5: return Vec3f(a, b, +t);
				default:
					assert(false);
			}
		}
	}
}

#endif /* LIGHTINGHELPERS_HPP_ */
