/*
 * GroundHeightLookup.cpp
 *
 *  Created on: Jun 12, 2011
 *      Author: david
 */

#include "GroundHeightLookup.h"
#include <Candy/Cubes/Cubes.h>

void GroundHeightLookup::Build(const Ptr(Cubes)& cubes)
{
	std::vector<Cell*> cells = cubes->GetCells();
	for(std::vector<Cell*>::iterator it=cells.begin(); it!=cells.end(); ++it) {
		Build(cubes, *it);
	}
}

void GroundHeightLookup::Build(const Ptr(Cubes)& cubes, Cell* cell)
{
	const CoordI wc = Properties::CellToWorld(cell->coordinate());
//	// check if already built by testing first cube
//	if(chunks_.find(Index(wcx0, wcy0)) != chunks_.end()) {
//		return;
//	}
	// handle each column
	for(unsigned int y=0; y<Properties::CellSize; y++) {
		int wy = wc.y + y;
		for(unsigned int x=0; x<Properties::CellSize; x++) {
			int wx = wc.x + x;
			Build(cubes, wx, wy);
		}
	}
}

int GroundHeightLookup::GetGroundZ(int x, int y, float z)
{
	std::map<Index,std::vector<int> >::const_iterator it=chunks_.find(Index(x,y));
	if(it == chunks_.end()) {
		// out of known area
		return 0;
	}
	const std::vector<int>& g = it->second;
	//
	//  g[2]
	//       XXXXXXX
	//  g[1] XXXXXXX
	//
	//
	//  g[0]
	//       XXXXXXX
	int g_last = g[0];
	for(size_t i=1; i<g.size(); i+=2) {
		int g_low = g[i];
		int g_high = g[i+1];
		float g_mean = float(g_low + g_high) * 0.5f;
		if(z < g_mean) {
			return g_last;
		}
		g_last = g_high;
	}
	return g_last;
}

std::vector<int> GroundHeightLookup::ComputeHeightLevels(const Ptr(Cubes)& cubes, int x, int y)
{
	// we assert that columns are always completely built!
	std::vector<int> z;
	Ci cw(x, y, z_min_);
	// move to level bottom
	while(cubes->GetType(cw) == CubeTypes::NonExistent && cw.z < z_max_) {
		cw.z++;
	}
	while(cw.z <= z_max_) {
		// seach next empty
		while(IsSolid(cubes->GetType(cw)) && cw.z < z_max_) {
			cw.z++;
		}
		if(!IsExisting(cubes->GetType(cw)) || cw.z == z_max_) {
			break;
		}
		// add empty
		z.push_back(cw.z);
		// seach next solid
		while(IsEmpty(cubes->GetType(cw)) && cw.z < z_max_) {
			cw.z++;
		}
		if(!IsExisting(cubes->GetType(cw)) || cw.z == z_max_) {
			break;
		}
		// add solid
		z.push_back(cw.z);
	}
	if(z.size() % 2 == 0) {
		// if it stops with solid, add first non-existing as empty
		z.push_back(cw.z);
	}
	assert(z.size() > 0 && z.size() % 2 == 1);
	return z;
}
