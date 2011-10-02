/*
 * PointWiseGenerator.h
 *
 *  Created on: 05.08.2011
 *      Author: david
 */

#ifndef CANDY_CUBES_POINTWISEGENERATOR_H_
#define CANDY_CUBES_POINTWISEGENERATOR_H_

#include <Candy/Tools/Ptr.h>
#include "WorldSize.h"
#include "../Generator.h"
#include "../Cell.h"
#include "../Cubes.h"

/**
 * G provides CubeType operator()(const CoordI& world)
 */
template<typename G>
class PointWiseGenerator
: public Generator
{
public:
	virtual ~PointWiseGenerator() {}

	PointWiseGenerator(const G& gen)
	: gen_(gen) {}

//	bool IsInfinite() {
//		return G::IsInfinite();
//	}
//
//	bool IsCellInRange(const CoordI& cell_coordinate) {
//		return G::IsCellInRange(cell_coordinate);
//	}

	void Generate(Cell* cell) {
		CoordI cw_start = Properties::CellToWorld(cell->coordinate());
		CoordI cw_end = cw_start + CoordI(Properties::CellSize, Properties::CellSize, Properties::CellSize);
		CoordI p;
		unsigned int index = 0;
		for(p.z=cw_start.z; p.z<cw_end.z; p.z++) {
			for(p.y=cw_start.y; p.y<cw_end.y; p.y++) {
				for(p.x=cw_start.x; p.x<cw_end.x; p.x++, index++) {
					CubeType t = gen_(p);
					cell->Set(index, t);
				}
			}
		}
	}

	void Generate(Cubes* cubes, const WorldSize& ws) {
		for(int x=ws.x1; x<ws.x2; x++) {
			for(int y=ws.y1; y<ws.y2; y++) {
				for(int z=ws.z1; z<ws.z2; z++) {
					Cell* cell = cubes->GetCell(CoordI(x,y,z));
					Generate(cell);
				}
			}
		}
	}

private:
	G gen_;

};

#endif
