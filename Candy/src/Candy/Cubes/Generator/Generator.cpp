/*
 * Generator.cpp
 *
 *  Created on: 06.08.2011
 *      Author: david
 */


#include "../Generator.h"
#include "PointWiseGenerator.h"
#include "CornellBox.h"
#include "PerlinHills.h"
#include "HillsAndFly.h"
#include "Terra.h"
#include "Perlin3D.h"
#include "City.h"
#include "SphericalTerra.h"

namespace Generators
{

#define FACTOR_HELPER(Gen) \
	Ptr(Generator) Factor##Gen()\
	{\
		typedef PointWiseGenerator<Gen> G;\
		return Ptr(G)(new G(Gen()));\
	}

#define FACTOR_HELPER_C1(Gen,T1) \
	Ptr(Generator) Factor##Gen(T1 a)\
	{\
		typedef PointWiseGenerator<Gen> G;\
		return Ptr(G)(new G(Gen(a)));\
	}

	FACTOR_HELPER_C1(CornellBox,unsigned int)
	FACTOR_HELPER(PerlinHills)
	FACTOR_HELPER(HillsAndFly)
	FACTOR_HELPER(Terra)
	FACTOR_HELPER(Perlin3D)
	FACTOR_HELPER(City)
	FACTOR_HELPER(SphericalTerra)

}
