/*
 * Lighting.hpp
 *
 *  Created on: 11.08.2011
 *      Author: david
 */

#ifndef LIGHTING_HPP_
#define LIGHTING_HPP_

#include <Candy/Tools/Ptr.h>
class Cubes;

namespace Hexa
{
	/** Base class for global illumination */
	class GlobalIlluminator
	{
	public:
		virtual ~GlobalIlluminator() {}

		virtual void Restart() {}

		virtual uint64_t Iterate(Ptr(Cubes) cubes) = 0;

	};

}

#endif
