#pragma once

#include "../Lighting.hpp"

class Cubes;
struct Cell;

namespace Hexa
{
namespace Lighting
{
	/** Global illumination with primitive Monte Carlo ray tracing */
	class MCRT
	: public GlobalIlluminator
	{
	public:
		uint64_t Iterate(Ptr(Cubes) cubes);
	};

}}
