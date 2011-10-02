#pragma once

#include <Candy/Candy_Dll.h>

namespace Perlin
{
	// Classic Perlin noise in 3D
	// see http://staffwww.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf

	CANDY_API void Initialize(unsigned int seed);

	CANDY_API double Noise(double x, double y);

	CANDY_API double Noise(double x, double y, double z);

	CANDY_API double Noise01(double x, double y);

	CANDY_API double Noise01(double x, double y, double z);

	CANDY_API double NoiseMP(double x, double y);

	CANDY_API double NoiseMP(double x, double y, double z);

	CANDY_API double NoiseMPScl(double x, double y, double z, double scl);

}
