#pragma once

#include <Candy/Tools/Coord.h>
#include <Candy/Tools/LinAlg.h>
#include <Candy/Tools/Math.h>
#include <Candy/Tools/Random.h>
#include <cmath>
#include <cassert>


typedef unsigned short BorderSideId;

typedef unsigned int CubeId;

#define VERTEX_2_INDEX(x,y,z) ((x) + (y) * (Common::CellVertexCount + (z) * Common::CellVertexCount))

namespace Common
{
	const unsigned int CellSize = 16;
	
	const unsigned int CellSizeMask = 15;

	const unsigned int CellSizeShift = 4;

	const unsigned int CellCubeCount = CellSize * CellSize * CellSize;

	const unsigned int CellVertexCount = CellSize + 1;

	const unsigned int BorderIdBits = 3;

	const unsigned int BorderIdMask = 7;

	const float CubeSize = 1.0f;

	inline unsigned int WorldToLocal(int w) {
		// solve w = a * n + b for b > 0 with n=CellSize and a an integer
		return w & CellSizeMask;
//		if(w >= 0) {
//			return (unsigned int)(w) & CellSizeMask;
//		} else {
//			return CellSize - (1 + ((unsigned int)(-w - 1) & CellSizeMask));
//		}
	}
	
	inline int WorldToCell(int w) {
//		if(w >= 0) {
//			return w >> CellSizeShift;
//		}
//		else {
//			return -((-w) >> CellSizeShift) - 1;
//		}
		if(w >= 0) {
			return int((unsigned int)(w) >> CellSizeShift);
		} else {
			return -int((1 + ((unsigned int)(-w - 1) >> CellSizeShift)));
		}
	}

	inline void WorldToCellLocal(int w, int& c_cell, unsigned int& c_local) {
//		c_local = w & CellSizeMask;
//		if(w >= 0) {
//			c_cell = w >> CellSizeShift;
//		}
//		else {
//			c_cell = -((-w) >> CellSizeShift) - 1;
//		}
		if(w >= 0) {
			unsigned int uw = (unsigned int)(w);
			c_cell = int(uw >> CellSizeShift);
			c_local = uw & CellSizeMask;
		} else {
			unsigned int uw = (unsigned int)(-w - 1);
			c_cell =  -int((1 + (uw >> CellSizeShift)));
			c_local = CellSize - (1 + (uw & CellSizeMask));
		}
	}

	inline unsigned int LocalToIndex(const Cu& c) {
		// FIXME optimize?
		unsigned int x = WorldToLocal(c.x);
		unsigned int y = WorldToLocal(c.y);
		unsigned int z = WorldToLocal(c.z);
		return x + CellSize * (y + CellSize * z);
	}

	inline void BorderSideToLocalSide(unsigned short c, Cu& c_local, unsigned int& side) {
		side = c & BorderIdMask;
		c_local.x = (c >> BorderIdBits) & CellSizeMask;
		c_local.y = (c >> (BorderIdBits + CellSizeShift)) & CellSizeMask;
		c_local.z = (c >> (BorderIdBits + 2*CellSizeShift)) & CellSizeMask;
	}

	inline unsigned int BorderSideToIndex(unsigned short c) {
		return c >> BorderIdBits;
	}

	/** Builds the identifier of a cube side
	 * @param c_local local cube coordinate
	 * @param side side index of the side [0,6[
	 *
	 * The low bits contain the side index. Higher bits contain the (x,y,z)
	 * coordinate. Border ids can be sorted.
	 */
	inline unsigned short BuildBorderSide(const Cu& c_local, unsigned int side) {
		return (unsigned short)(
			side + (c_local.x << BorderIdBits) + (c_local.y << (BorderIdBits + CellSizeShift)) + (c_local.z << (BorderIdBits + 2*CellSizeShift))
		);
	}

	inline unsigned int WorldToIndex(const Ci& c) {
		// FIXME optimize?
		return c.x + CellSize * (c.y + CellSize * c.z);
	}

	inline Cu WorldToLocal(const Ci& c) {
		return CoordU(
			WorldToLocal(c.x),
			WorldToLocal(c.y),
			WorldToLocal(c.z)
		);
	}

	inline Ci LocalToWorld(const Ci& c_cell, const Cu& c_local) {
		return CoordI(
			CellSize * c_cell.x + int(c_local.x),
			CellSize * c_cell.y + int(c_local.y),
			CellSize * c_cell.z + int(c_local.z));
	}

	inline Ci LocalToWorld(const Ci& c_cell, const Ci& c_local) {
		return CoordI(
			CellSize * c_cell.x + c_local.x,
			CellSize * c_cell.y + c_local.y,
			CellSize * c_cell.z + c_local.z);
	}

	inline Ci WorldToCell(const Ci& c) {
		return CoordI(
			WorldToCell(c.x),
			WorldToCell(c.y),
			WorldToCell(c.z)
		);
	}

	inline void WorldToCellLocal(const Ci& c, Ci& c_cell, Cu& c_local) {
		WorldToCellLocal(c.x, c_cell.x, c_local.x);
		WorldToCellLocal(c.y, c_cell.y, c_local.y);
		WorldToCellLocal(c.z, c_cell.z, c_local.z);
	}

	inline int CellLocalToWorld(int cell, unsigned int local) {
		 return int(CellSize) * cell + (int)local;
	}

	inline void CellLocalToWorld(const Ci& c_cell, const Cu& c_local, Ci& c_world) {
		c_world.x = CellLocalToWorld(c_cell.x, c_local.x);
		c_world.y = CellLocalToWorld(c_cell.y, c_local.y);
		c_world.z = CellLocalToWorld(c_cell.z, c_local.z);
	}

	inline void WorldToCellBorderSide(const Ci& c, unsigned int side, Ci& c_cell, BorderSideId& bs) {
		Cu c_local;
		WorldToCellLocal(c.x, c_cell.x, c_local.x);
		WorldToCellLocal(c.y, c_cell.y, c_local.y);
		WorldToCellLocal(c.z, c_cell.z, c_local.z);
		bs = BuildBorderSide(c_local, side);
	}

	inline unsigned int VertexToIndex(unsigned int x, unsigned int y, unsigned int z) {
		return VERTEX_2_INDEX(x, y, z);
	}

	inline Ci FirstInCell(const Ci& c) {
		return CoordI(int(CellSize) * c.x, int(CellSize) * c.y, int(CellSize) * c.z);
	}

	inline float WorldToPosition(int w) {
		return float(w);
	}

	inline Vec3f CellMin(const Ci& c) {
		return Vec3f(
			WorldToPosition(int(CellSize) * c.x),
			WorldToPosition(int(CellSize) * c.y),
			WorldToPosition(int(CellSize) * c.z));
	}

	inline Vec3f CellMid(const Ci& c) {
		return Vec3f(
			WorldToPosition(int(CellSize) * c.x) + float(CellSize) * 0.5f,
			WorldToPosition(int(CellSize) * c.y) + float(CellSize) * 0.5f,
			WorldToPosition(int(CellSize) * c.z) + float(CellSize) * 0.5f);
	}

	inline Vec3f CellMax(const Ci& c) {
		return Vec3f(
			WorldToPosition(int(CellSize) * (c.x + 1)),
			WorldToPosition(int(CellSize) * (c.y + 1)),
			WorldToPosition(int(CellSize) * (c.z + 1)));
	}

	inline void WorldToPositionCenter(const Ci& cc_world, Vec3f& position) {
		position(0) = WorldToPosition(cc_world.x) + 0.5f;
		position(1) = WorldToPosition(cc_world.y) + 0.5f;
		position(2) = WorldToPosition(cc_world.z) + 0.5f;
	}

	inline float WorldToPositionCenter(int x) {
		return WorldToPosition(x) + 0.5f;
	}

	inline Vec3f WorldToPositionCenter(int x, int y, int z) {
		return Vec3f(
			WorldToPosition(x) + 0.5f,
			WorldToPosition(y) + 0.5f,
			WorldToPosition(z) + 0.5f
		);
	}

	inline Vec3f WorldToPositionCenter(const Ci& cc_world) {
		return WorldToPositionCenter(cc_world.x, cc_world.y, cc_world.z);
	}

	inline int PositionToInt(float p) {
		return (p < 0) ? int(p) - 1 : int(p); // must round down!
	}

	inline int PositionToInt(float p, float& rest) {
		int pi = PositionToInt(p);
		rest = p - float(pi);
		return pi;
	}

	inline Ci PositionToInt(float x, float y, float z) {
		return Ci(
			PositionToInt(x),
			PositionToInt(y),
			PositionToInt(z)
		);
	}

	inline Ci PositionToInt(const Vec3f& position) {
		return Ci(
			PositionToInt(position(0)),
			PositionToInt(position(1)),
			PositionToInt(position(2))
		);
	}

	inline Ci PositionToInt(const Vec3f& position, Vec3f& rest) {
		return Ci(
			PositionToInt(position(0), rest[0]),
			PositionToInt(position(1), rest[1]),
			PositionToInt(position(2), rest[2])
		);
	}

	inline void PositionToInt(const Vec3f& position, Ci& cc_world) {
		cc_world.x = PositionToInt(position(0));
		cc_world.y = PositionToInt(position(1));
		cc_world.z = PositionToInt(position(2));
	}

}

namespace Geometry
{
	const float normals[6][3] = {
		{0, 0, -1}, // 0
		{-1, 0, 0}, // 1
		{0, +1, 0}, // 2
		{+1, 0, 0}, // 3
		{0, -1, 0}, // 4
		{0, 0, +1}  // 5
	};

	const int Tangential[6][3] = {
		{1, 0, 2},
		{2, 1, 0},
		{2, 0, 1},
		{1, 2, 0},
		{0, 2, 1},
		{0, 1, 2}
	};

	inline int SideIndexX(int sign) {
		return (sign < 0) ? 1 : 3;
	}

	inline int SideIndexY(int sign) {
		return (sign < 0) ? 4 : 2;
	}

	inline int SideIndexZ(int sign) {
		return (sign < 0) ? 0 : 5;
	}

	inline int TopSide() {
		return 5;
	}

	inline void SideNormal(int side, float& nx, float& ny, float& nz) {
		nx = normals[side][0];
		ny = normals[side][1];
		nz = normals[side][2];
	}

	inline Vec3f SideNormal(int side) {
		return Vec3f(normals[side][0], normals[side][1], normals[side][2]);
	}

	const int VertexPositionOffset[8][3] = {
		// FIXME there is a repetition of this in VertexIndexOffset!
		{0,0,0}, // 0
		{0,1,0}, // 1
		{1,1,0}, // 2
		{1,0,0}, // 3
		{0,0,1}, // 4
		{0,1,1}, // 5
		{1,1,1}, // 6
		{1,0,1}  // 7
	};

	inline Vec3f CellCorner(const CoordI& c, unsigned int i) {
		assert(i < 8);
		return Vec3f(float(int(Common::CellSize) * (c.x + VertexPositionOffset[i][0])),
					 float(int(Common::CellSize) * (c.y + VertexPositionOffset[i][1])),
					 float(int(Common::CellSize) * (c.z + VertexPositionOffset[i][2]))
		);
	}

	const int SideVertexIndices[6][4] = {
		{0,3,2,1}, // z- bottom
		{0,1,5,4}, // x-
		{1,2,6,5}, // y+
		{2,3,7,6}, // x+
		{3,0,4,7}, // y-
		{4,5,6,7}  // z+ top
	};

	const int VertexIndexOffset[8] = {
		// FIXME this is a repetition of VertexPositionOffset!
		VERTEX_2_INDEX(0,0,0),
		VERTEX_2_INDEX(0,1,0),
		VERTEX_2_INDEX(1,1,0),
		VERTEX_2_INDEX(1,0,0),
		VERTEX_2_INDEX(0,0,1),
		VERTEX_2_INDEX(0,1,1),
		VERTEX_2_INDEX(1,1,1),
		VERTEX_2_INDEX(1,0,1)
	};

	inline int SideVertexIndexOffset(unsigned int sideIndex, unsigned int vertexIndex) {
		return VertexIndexOffset[SideVertexIndices[sideIndex][vertexIndex]];
	}

	const int SideNeighbourDirection[6][3] = {
		{0,0,-1},
		{-1,0,0},
		{0,+1,0},
		{+1,0,0},
		{0,-1,0},
		{0,0,+1},
	};


	inline CoordI GetSideNeighbour(const CoordI& cc_world, int side) {
		return CoordI(
			cc_world.x + SideNeighbourDirection[side][0],
			cc_world.y + SideNeighbourDirection[side][1],
			cc_world.z + SideNeighbourDirection[side][2]
		);
	}

	inline void GetVertexPosition(const CoordI& cc_world, int side, int vertex, float& px, float& py, float& pz) {
		px = Common::WorldToPosition(cc_world.x + VertexPositionOffset[SideVertexIndices[side][vertex]][0]);
		py = Common::WorldToPosition(cc_world.y + VertexPositionOffset[SideVertexIndices[side][vertex]][1]);
		pz = Common::WorldToPosition(cc_world.z + VertexPositionOffset[SideVertexIndices[side][vertex]][2]);
	}
}

namespace Exceptions
{
	struct InvalidCell {};
}
