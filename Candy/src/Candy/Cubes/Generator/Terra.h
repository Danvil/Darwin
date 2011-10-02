/*
 * Terra.h
 *
 *  Created on: May 20, 2011
 *      Author: david
 */

#ifndef TERRA_H_
#define TERRA_H_

#include <Candy/Tools/Perlin.h>
#include <Candy/Tools/FunctionCache.h>

namespace Generators
{
	struct Terra
	{
		static float sigmoid_impl(float x)  {
			return 1.0f / (1.0f + std::exp(-x));
		}

		static float sigmoid(float x)  {
			static Danvil::FunctionCache<float> c = Danvil::FunctionCache<float>(-12.0f, +12.0f, &sigmoid_impl);
			return c(x);
			//return sigmoid_impl(x);
		}

		static float sigmoid(float x, float a, float b)  {
			return a + sigmoid(x) * (b - a);
		}

		float p_cavy(float x, float y, float z) {
			const float cZInfluence = 2.0f / 12.0f;
			const float cZOffset = +8.0f;
			const float cScale = 0.08f;
			float p = Perlin::Noise01(cScale * x, cScale * y, cScale * z);
			return sigmoid(cZInfluence*(cZOffset + z), 0.5f + 0.5f*p, 1.0f);
		}

		float p_cliffy(float x, float y, float z) {
			const float cScale = 0.08f;
			const float cScale2 = 0.8f;
			const float cZInfluence = 2.0f / 12.0f;
			const float cZOffset = -16.0f;
			const float cD = 0.3f;
			float q = Perlin::Noise01(cScale * x, cScale * y, cScale * z);
			//float q2 = Perlin::Noise01(cScale2 * x, cScale2 * y, cScale2 * z);
			//q *= 1.0f + 0.1f * q2;
			float s = sigmoid(-cZInfluence*(cZOffset + z));
			float s1 = cD + s*(1 - cD);
			float s2 = cD * s;
			return s1 + q * (s2 - s1);
		}

		float p_plainy(float x, float y, float z) {
			const float cScale = 0.03f;
			const float cScale2 = 0.5f;
			const float cAmplitude = 8.0f;
			const float cAmplitude2 = 0.65f;
			float q = cAmplitude * Perlin::NoiseMP(cScale * x, cScale * y) - z;
			float q2 = cAmplitude2 * Perlin::NoiseMP(cScale2 * x, cScale2 * y);
			return sigmoid(q + q2);
		}

		float p_watery(float x, float y, float z) {
			const float cScale = 0.06f;
			float q = Perlin::NoiseMP(cScale * x, cScale * y, cScale * z);
			return q;
		}

		static const int z1 = -2;
		static const int z2 = +2;

		int world_bottom_border() const {
			return int(Common::CellSize) * z1;
		}

		int world_top_border() const {
			return int(Common::CellSize) * z2 - 1;
		}

		bool is_bottom_border(int z) const {
			return z == world_bottom_border();
		}

		bool is_top_border(int z) const {
			return z == world_top_border();
		}

		bool is_bottom(int z) const {
			return z < world_bottom_border();
		}

		static const float cWorldScale = 1.0f;

		CubeType operator()(const CoordI& cw) {
			if(is_bottom_border(cw.z)) {
				return CubeTypes::Indestructible;
			}
			if(is_bottom_border(cw.z - 1)) {
				return CubeTypes::Lava;
			}
			if(is_top_border(cw.z) || is_top_border(cw.z + 1)) {
				return CubeTypes::Empty;
			}
			float x = cWorldScale * Common::WorldToPositionCenter(cw.x);
			float y = cWorldScale * Common::WorldToPositionCenter(cw.y);
			float z = cWorldScale * Common::WorldToPositionCenter(cw.z);
			float v_cliffy = p_cliffy(x, y, z);
			float v_plainy = p_plainy(x, y, z);
			float v_cavy = p_cavy(x, y, z);
			const float cScaleSelection = 0.02f;
			float sel = Perlin::NoiseMP(cScaleSelection * x, cScaleSelection * y);
			sel = sigmoid(20.0f*sel);
			//sel = sel > 0 ? 1.0f : 0.0f;
			float n = v_cavy * (sel * v_cliffy + (1 - sel) * v_plainy);
			// check if empty
			if(n < 0.5f) {
				return CubeTypes::Empty;
			}
			else {
//				// not empty
//				const float cScaleCrystal = 1.0f;
//				if(Perlin::Noise01(cScaleCrystal * x, cScaleCrystal * y, cScaleCrystal * z) < 0.2) {
//					// crystal
//					float p = sigmoid(float(cw.z) / 16.0f);
//					return Random::Uniformf() < p ? CubeTypes::CrystalGood : CubeTypes::CrystalEvil;
//				}
//				else {
//					// normal
					return Random::Uniformf() < sel ? CubeTypes::Stone : CubeTypes::Mud;
//				}
			}
		}
	};
}

#endif
