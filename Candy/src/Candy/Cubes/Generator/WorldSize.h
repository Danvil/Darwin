/*
 * WorldSize.h
 *
 *  Created on: Jun 12, 2011
 *      Author: david
 */

#ifndef WORLDSIZE_H_
#define WORLDSIZE_H_

#include <Candy/Cubes/Common.h>

struct WorldSize
{
	WorldSize() {
#ifdef NDEBUG
		x1 = -8;
		x2 = +8;
		y1 = -8;
		y2 = +8;
		z1 = -2;
		z2 = +4;
#else
		x1 = -1;
		x2 = +1;
		y1 = -1;
		y2 = +1;
		z1 = -1;
		z2 = +1;
#endif
	}

	WorldSize(int range) {
		x1 = -range;
		x2 = +range;
		y1 = -range;
		y2 = +range;
		z1 = -range;
		z2 = +range;
	}

	WorldSize(int range_xy, int range_z) {
		x1 = -range_xy;
		x2 = +range_xy;
		y1 = -range_xy;
		y2 = +range_xy;
		z1 = -range_z;
		z2 = +range_z;
	}

	WorldSize(int range_xy, int range_z_min, int range_z_max) {
		x1 = -range_xy;
		x2 = +range_xy;
		y1 = -range_xy;
		y2 = +range_xy;
		z1 = range_z_min;
		z2 = range_z_max;
	}

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

	bool is_top(int z) const {
		return z > world_top_border();
	}

	int world_x1() const {
		return int(Common::CellSize) * x1;
	}

	int world_x2() const {
		return int(Common::CellSize) * x2 - 1;
	}

	int world_y1() const {
		return int(Common::CellSize) * y1;
	}

	int world_y2() const {
		return int(Common::CellSize) * y2 - 1;
	}

	int cell_count() const {
		return (x2 - x1)*(y2 - y1)*(z2 - z1);
	}

	int cell_count_xy() const {
		return (x2 - x1)*(y2 - y1);
	}

	int x1, x2;
	int y1, y2;
	int z1, z2;
};

#endif /* WORLDSIZE_H_ */
