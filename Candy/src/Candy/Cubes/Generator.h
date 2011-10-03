/*
 * Generator.h
 *
 *  Created on: 05.08.2011
 *      Author: david
 */

#ifndef GENERATOR_H_
#define GENERATOR_H_

#include "Common.h"
#include "Cube.h"
#include "Generator/WorldSize.h"
#include <Candy/Tools/Ptr.h>

class Cell;
class Cubes;

class Generator
{
public:
	virtual ~Generator() {}

//	virtual bool IsInfinite() = 0;

//	virtual bool IsCellInRange(const CoordI& cell_coordinate) = 0;

	virtual void Generate(Cell* cell) = 0;

	virtual void Generate(Cubes* cubes, const WorldSize& ws) = 0;

};

namespace Generators {
	Ptr(Generator) FactorCornellBox(unsigned int size);
	Ptr(Generator) FactorPerlinHills();
	Ptr(Generator) FactorHillsAndFly();
	Ptr(Generator) FactorTerra();
	Ptr(Generator) FactorPerlin3D();
	Ptr(Generator) FactorCity();
	Ptr(Generator) FactorSphericalTerra();
	Ptr(Generator) FactorZeroHeight();
}

#endif /* GENERATOR_H_ */
