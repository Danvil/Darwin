/*
 * GroundHeightLookup.h
 *
 *  Created on: Jun 12, 2011
 *      Author: david
 */

#ifndef GROUNDHEIGHTLOOKUP_H_
#define GROUNDHEIGHTLOOKUP_H_

#include <Candy/Tools/Coord.h>
#include <Candy/Tools/Ptr.h>
#include <map>
#include <vector>

class Cubes;
struct Cell;

class GroundHeightLookup
{
public:
	struct Index {
		int x, y;
		Index() {}
		Index(int nx, int ny) : x(nx), y(ny) {}
	};

	void SetHeightRange(int z) {
		z_min_ = -z;
		z_max_ = +z;
	}

	void Build(const Ptr(Cubes)& cubes);

	void Build(const Ptr(Cubes)& cubes, Cell* cell);

	void Build(const Ptr(Cubes)& cubes, const CoordI& c_cell);

	void Build(const Ptr(Cubes)& cubes, int x, int y) {
		chunks_[Index(x, y)] = ComputeHeightLevels(cubes, x, y);
	}

	int GetGroundZ(int x, int y, float z) const;

private:
	std::vector<int> ComputeHeightLevels(const Ptr(Cubes)& cubes, int x, int y);

private:
	int z_min_, z_max_;

	std::map<Index, std::vector<int> > chunks_;

};

inline bool operator<(const GroundHeightLookup::Index& i, const GroundHeightLookup::Index& j) {
	return i.y < j.y || (i.y == j.y && i.x < j.x);
}

#endif /* GROUNDHEIGHTLOOKUP_H_ */
