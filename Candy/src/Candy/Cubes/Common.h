#pragma once

#include <Candy/Tools/Coord.h>
#include <Candy/Tools/LinAlg.h>
#include <Candy/Tools/Math.h>
#include <Candy/Tools/Random.h>
#include <Eigen/Geometry>
#include <cmath>
#include <cassert>

typedef unsigned short BorderSideId;

typedef unsigned int CubeId;

namespace CandyCubes
{

	/** For x an integer returns y s.t. x = y*N + a and 0 <= a < N */
	template<unsigned int Shift>
	inline int SignedShiftDiv(int x) {
		return x >> Shift; // arithmetic shift
//		if(x >= 0) {
//			return int((unsigned int)(x) >> Shift);
//		} else {
//			return -int((1 + ((unsigned int)(-x - 1) >> Shift)));
//		}
//		if(x >= 0) {
//			return x >> Shift;
//		}
//		else {
//			return -((-x) >> Shift) - 1;
//		}
	}

	/** For x an integer returns y s.t. x = a*N + y and 0 <= y < N */
	template<unsigned int Mask>
	inline unsigned int SignedMaskMod(int x) {
		return x & Mask;
//		if(x >= 0) {
//			return (unsigned int)(x) & Mask;
//		} else {
//			return CellSize - (1 + ((unsigned int)(-x - 1) & Mask));
//		}
	}

	struct BaseProperties
	{
		/** Used internally for arithmetics */
		static const unsigned int CellSizeShift = 4; // yields 16 cubes per cell

		/** Number of cubes along one side of a cell (must be a multiple of 2) */
		static const unsigned int CellSize = (1 << CellSizeShift);

		/** Used internally for arithmetics */
		static const unsigned int CellSizeMask = CellSize - 1;

		/** Number of cubes per cell */
		static const unsigned int CellCubeCount = CellSize * CellSize * CellSize;

		/** Number of vertices along one side of a cell (=one more than cubes) */
		static const unsigned int CellVertexSize = CellSize + 1;

		/** Number of vertices per cell */
		static const unsigned int CellVertexCount = CellVertexSize * CellVertexSize * CellVertexSize;

		/** Number of bits needed to encode the side id of a cube (=3) */
		static const unsigned int BorderIdBits = 3;

		/** Used internally for arithmetics */
		static const unsigned int BorderIdMask = (1 << BorderIdBits) - 1;

		/** Size of a cube in the world. Do not change as the code uses 1.0f implicitly! */
		static const float CubeSize = 1.0f;

//		/** Converts a coordinate to an index */
//		static inline unsigned int WorldToIndex(const Ci& c) {
//			// FIXME optimize?
//			return c.x + CellSize * (c.y + CellSize * c.z);
//		}

		/** Computes index of a cube in an zyx ordered array of all cell cubes */
		static inline unsigned int LocalToIndex(unsigned int lx, unsigned int ly, unsigned int lz) {
			return (lx + CellSize * (ly + CellSize * lz));
		}

		/** Computes index of a cube corner vertex in an zyx ordered array of all cell cube corner vertices */
		static inline unsigned int VertexToIndex(unsigned int vx, unsigned int vy, unsigned int vz) {
			return (vx + CellVertexSize * (vy + CellVertexSize * vz));
		}

		static inline unsigned int LocalToIndex(const Cu& cl) {
			return LocalToIndex(cl.x, cl.y, cl.z);
		}

		static inline void BorderSideToLocalSide(unsigned short c, Cu& c_local, unsigned int& side) {
			side = c & BorderIdMask;
			c_local.x = (c >> BorderIdBits) & CellSizeMask;
			c_local.y = (c >> (BorderIdBits + CellSizeShift)) & CellSizeMask;
			c_local.z = (c >> (BorderIdBits + 2*CellSizeShift)) & CellSizeMask;
		}

		static inline unsigned int BorderSideToIndex(unsigned short c) {
			return c >> BorderIdBits;
		}

		/** Builds the identifier of a cube side
		 * @param c_local local cube coordinate
		 * @param side side index of the side [0,6[
		 *
		 * The low bits contain the side index. Higher bits contain the (x,y,z)
		 * coordinate. Border ids can be sorted.
		 */
		static inline unsigned short BuildBorderSide(const Cu& c_local, unsigned int side) {
			return (unsigned short)(
				side + (c_local.x << BorderIdBits) + (c_local.y << (BorderIdBits + CellSizeShift)) + (c_local.z << (BorderIdBits + 2*CellSizeShift))
			);
		}

		/** Gets the tangential directions for a side
		 * @param side_id side index [0, 5[
		 * @param tangential_dir_1 first tangential direction
		 * @param tangential_dir_2 second tangential direction
		 * @param normal_dir normal direction
		 */
		static inline void GetTangential(unsigned int side_id, unsigned int& tangential_dir_1, unsigned int& tangential_dir_2, unsigned int& normal_dir) {
			static const int cTangentialData[6][3] = {
				{1, 0, 2},
				{2, 1, 0},
				{2, 0, 1},
				{1, 2, 0},
				{0, 2, 1},
				{0, 1, 2}
			};
			tangential_dir_1 = cTangentialData[side_id][0];
			tangential_dir_2 = cTangentialData[side_id][1];
			normal_dir = cTangentialData[side_id][2];
		}

		static inline int SideIndexX(int sign) {
			return (sign < 0) ? 1 : 3;
		}

		static inline int SideIndexY(int sign) {
			return (sign < 0) ? 4 : 2;
		}

		static inline int SideIndexZ(int sign) {
			return (sign < 0) ? 0 : 5;
		}

		static inline int TopSide() {
			return 5;
		}

		static inline unsigned int GetVertexPositionOffset(unsigned int vertex_id, unsigned int i) {
			const int cVertexPositionOffsetData[8][3] = {
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
			assert(vertex_id < 8);
			assert(i < 3);
			return cVertexPositionOffsetData[vertex_id][i];
		}

		static inline void GetVertexPositionOffsets(int vertex_id, int& dx, int& dy, int& dz) {
			dx = GetVertexPositionOffset(vertex_id, 0);
			dy = GetVertexPositionOffset(vertex_id, 1);
			dz = GetVertexPositionOffset(vertex_id, 2);
		}

		static inline CoordI GetVertexCoordOffset(int vertex_id) {
			return CoordI(
					GetVertexPositionOffset(vertex_id, 0),
					GetVertexPositionOffset(vertex_id, 1),
					GetVertexPositionOffset(vertex_id, 2));
		}

		static inline unsigned int GetSideVertexIndices(unsigned int vertex_id, unsigned int i) {
			assert(vertex_id < 8);
			assert(i < 3);
			const unsigned int cSideVertexIndices[8][3] = {
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
			return cSideVertexIndices[vertex_id][i];
		}

		static inline void GetSideVertexIndices(unsigned int vertex_id, unsigned int& dx, unsigned int& dy, unsigned int& dz) {
			dx = GetSideVertexIndices(vertex_id, 0);
			dy = GetSideVertexIndices(vertex_id, 1);
			dz = GetSideVertexIndices(vertex_id, 2);
		}

		static inline unsigned int GetSideVertexIndexOffset(unsigned int vertex_id) {
			unsigned int dx, dy, dz;
			GetSideVertexIndices(vertex_id, dx, dy, dz);
			return VertexToIndex(dx, dy, dz);
		}

		static inline unsigned int GetSideVertexData(unsigned int side_id, unsigned int i) {
			static const unsigned int cSideVertices[6][4] = {
				{0,3,2,1}, // z- bottom
				{0,1,5,4}, // x-
				{1,2,6,5}, // y+
				{2,3,7,6}, // x+
				{3,0,4,7}, // y-
				{4,5,6,7}  // z+ top
			}; // FIXME duplicate!
			assert(side_id < 6);
			assert(i < 4);
			return cSideVertices[side_id][i];
		}

		static inline void GetSideVertices(unsigned int vertex_id, unsigned int& a, unsigned int& b, unsigned int& c, unsigned int& d) {
			a = GetSideVertexData(vertex_id, 0);
			b = GetSideVertexData(vertex_id, 1);
			c = GetSideVertexData(vertex_id, 2);
			d = GetSideVertexData(vertex_id, 3);
		}

		static inline unsigned int SideVertexIndexOffset(unsigned int side_id, unsigned int i) {
			return GetSideVertexIndexOffset(GetSideVertexData(side_id, i));
		}

		static inline CoordI GetSideNeighbour(const CoordI& cc_world, int side) {
			int SideNeighbourDirection[6][3] = {
				{0,0,-1},
				{-1,0,0},
				{0,+1,0},
				{+1,0,0},
				{0,-1,0},
				{0,0,+1},
			};
			return CoordI(
				cc_world.x + SideNeighbourDirection[side][0],
				cc_world.y + SideNeighbourDirection[side][1],
				cc_world.z + SideNeighbourDirection[side][2]
			);
		}

		static inline CoordI GetVertexCoord(const CoordI& cc_world, int side_id, int i) {
			int vertex_id = GetSideVertexData(side_id, i);
			CoordI offset = GetVertexCoordOffset(vertex_id);
			return cc_world + offset;
		}

		/** Computes a cube side point
		 * @param side_index the side of the cube (specifies the normal)
		 * @param h distance to the cube center in the direction of the side normal
		 * @param u distance to the cube center in the direction of the first side tangential
		 * @param v distance to the cube center in the direction of the second side tangential
		 * @param the cube point
		 */
		static inline Vec3f CubeSidePointBase(unsigned int side_index, float h, float u, float v) {
			assert(side_index < 6);
			switch(side_index) {
				case 0: return Vec3f(u, v, -h);
				case 1: return Vec3f(-h, u, v);
				case 2: return Vec3f(u, +h, v);
				case 3: return Vec3f(+h, u, v);
				case 4: return Vec3f(u, -h, v);
				case 5: return Vec3f(u, v, +h);
				default: return Vec3f::Zero();
			}
		}

	};

	/**
	 * Base must provide the following functions:
	 * - derived from BaseProperties
	 * - static void WorldToLocal(int wx, int wy, int wz, unsigned int& lx, unsigned int& ly, unsigned int& lz);
	 * - static void WorldToCell(int wx, int wy, int wz, int& cx, int& cy, int& cz);
	 * - static void CellLocalToWorld(int cx, int cy, int cz, unsigned int lx, unsigned int ly, unsigned int lz, int& wx, int& wy, int& wz);
	 * - static void WorldToPosition(int wx, int wy, int wz, float& px, float& py, float& pz);
	 * - static void WorldToPositionCenter(int vx, int vy, int vz, float& x, float& y, float& z);
	 * - static void PositionToWorld(float px, float py, float pz, int& wx, int& wy, int&wz);
	 * - static void PositionToWorld(float px, float py, float pz, int& wx, int& wy, int&wz, float& rx, float& ry, float& rz);
	 * - static void GetSideNormal(const CoordI&, int side, float& nx, float& ny, float& nz);
	 */
	template<typename Base>
	struct DerivedProperties
	: public Base
	{
		static inline Cu WorldToLocal(const Ci& c_world) {
			unsigned int lx, ly, lz;
			Base::WorldToLocal(c_world.x, c_world.y, c_world.z, lx, ly, lz);
			return CoordU(lx, ly, lz);
		}

		static inline Ci WorldToCell(const Ci& c_world) {
			int cx, cy, cz;
			Base::WorldToCell(c_world.x, c_world.y, c_world.z, cx, cy, cz);
			return Ci(cx, cy, cz);
		}

		static inline void WorldToCellLocal(const Ci& c_world, Ci& c_cell, Cu& c_local) {
			Base::WorldToLocal(c_world.x, c_world.y, c_world.z, c_local.x, c_local.y, c_local.z);
			Base::WorldToCell(c_world.x, c_world.y, c_world.z, c_cell.x, c_cell.y, c_cell.z);
		}

		static inline Ci CellLocalToWorld(const Ci& c_cell, const Cu& c_local) {
			int wx, wy, wz;
			Base::CellLocalToWorld(c_cell.x, c_cell.y, c_cell.z, c_local.x, c_local.y, c_local.z, wx, wy, wz);
			return Ci(wx, wy, wz);
		}

		static inline Ci CellToWorld(const Ci& c_cell) {
			int wx, wy, wz;
			Base::CellLocalToWorld(c_cell.x, c_cell.y, c_cell.z, 0, 0, 0, wx, wy, wz);
			return Ci(wx, wy, wz);
		}

		static inline unsigned int WorldToIndex(const Ci& c_world) {
			unsigned int lx, ly, lz;
			Base::WorldToLocal(c_world.x, c_world.y, c_world.z, lx, ly, lz);
			return Base::LocalToIndex(lx, ly, lz);
		}

		static inline void WorldToCellBorderSide(const Ci& c_world, unsigned int side, Ci& c_cell, BorderSideId& bs) {
			Cu c_local;
			WorldToCellLocal(c_world, c_cell, c_local);
			bs = Base::BuildBorderSide(c_local, side);
		}

		static inline void WorldToPosition(const CoordI& c_world, float& x, float& y, float& z) {
			Base::WorldToPosition(c_world.x, c_world.y, c_world.z, x, y, z);
		}

		static inline Vec3f WorldToPosition(int vx, int vy, int vz) {
			float x, y, z;
			Base::WorldToPosition(vx, vy, vz, x, y, z);
			return Vec3f(x, y, z);
		}

		static inline Vec3f WorldToPosition(const CoordI& c_world) {
			return WorldToPosition(c_world.x, c_world.y, c_world.z);
		}

		static inline Vec3f CellMin(const Ci& c_world) {
			return WorldToPosition(
					int(Base::CellSize) * c_world.x,
					int(Base::CellSize) * c_world.y,
					int(Base::CellSize) * c_world.z);
		}

		static inline Vec3f CellMax(const Ci& c_world) {
			return WorldToPosition(
					int(Base::CellSize) * (c_world.x + 1),
					int(Base::CellSize) * (c_world.y + 1),
					int(Base::CellSize) * (c_world.z + 1));
		}

		static inline Vec3f CellMid(const Ci& c) {
			return 0.5f * (CellMin(c) + CellMax(c));
		}

		static inline Vec3f WorldToPositionCenter(int wx, int wy, int wz) {
			float x, y, z;
			Base::WorldToPositionCenter(wx, wy, wz, x, y, z);
			return Vec3f(x, y, z);
		}

		static inline Vec3f WorldToPositionCenter(const Ci& cc_world) {
			return WorldToPositionCenter(cc_world.x, cc_world.y, cc_world.z);
		}

		static inline void WorldToPositionCenter(const Ci& cc_world, float& x, float& y, float& z) {
			Base::WorldToPositionCenter(cc_world.x, cc_world.y, cc_world.z, x, y, z);
		}

		static inline void PositionToWorld(float px, float py, float pz, int& wx, int& wy, int&wz) {
			// FIXME why??
			Base::PositionToWorld(px, py, pz, wx, wy, wz);
		}

		static inline Ci PositionToWorld(float x, float y, float z) {
			int wx, wy, wz;
			Base::PositionToWorld(x, y, z, wx, wy, wz);
			return Ci(wx, wy, wz);
		}

		static inline Ci PositionToWorld(const Vec3f& p) {
			return PositionToWorld(p[0], p[1], p[2]);
		}

		static inline Ci PositionToWorld(const Vec3f& p, Vec3f& rest) {
			int wx, wy, wz;
			Base::PositionToWorld(p[0], p[1], p[2], wx, wy, wz, rest[0], rest[1], rest[2]);
			return Ci(wx, wy, wz);
		}

		static inline Vec3f SideNormal(const CoordI& cw, int side) {
			float nx, ny, nz;
			Base::GetSideNormal(cw, side, nx, ny, nz);
			return Vec3f(nx, ny, nz);
		}

		static inline Vec3f CellCorner(const CoordI& c, unsigned int vertex_id) {
			int dx, dy, dz;
			Base::GetVertexPositionOffsets(vertex_id, dx, dy, dz);
			return WorldToPosition(
					int(Base::CellSize) * (c.x + dx),
					int(Base::CellSize) * (c.y + dy),
					int(Base::CellSize) * (c.z + dz));
		}

	};

	struct PlanarProperties
	: public BaseProperties
	{
	private:
		static inline unsigned int WorldToLocal(int w) {
			return SignedMaskMod<CellSizeMask>(w);
		}

		static inline int WorldToCell(int w) {
			return SignedShiftDiv<CellSizeShift>(w);
		}

		static inline int CellLocalToWorld(int cell, unsigned int local) {
			 return int(CellSize) * cell + int(local);
		}

		static inline int PositionToWorld(float p) {
			return (p < 0) ? int(p) - 1 : int(p); // must round down!
		}

		static inline int PositionToWorld(float p, float& rest) {
			int pi = PositionToWorld(p);
			rest = p - float(pi);
			return pi;
		}

		static inline float WorldToPosition(int w) {
			return float(w);
		}

		static inline float WorldToPositionCenter(int x) {
			return WorldToPosition(x) + 0.5f;
		}

	public:
		static void WorldToLocal(int wx, int wy, int wz, unsigned int& lx, unsigned int& ly, unsigned int& lz) {
			lx = WorldToLocal(wx);
			ly = WorldToLocal(wy);
			lz = WorldToLocal(wz);
		}

		static void WorldToCell(int wx, int wy, int wz, int& cx, int& cy, int& cz) {
			cx = WorldToCell(wx);
			cy = WorldToCell(wy);
			cz = WorldToCell(wz);
		}

		static void CellLocalToWorld(int cx, int cy, int cz, unsigned int lx, unsigned int ly, unsigned int lz, int& wx, int& wy, int& wz) {
			wx = CellLocalToWorld(cx, lx);
			wy = CellLocalToWorld(cy, ly);
			wz = CellLocalToWorld(cz, lz);
		}

		static void WorldToPosition(int vx, int vy, int vz, float& x, float& y, float& z) {
			x = WorldToPosition(vx);
			y = WorldToPosition(vy);
			z = WorldToPosition(vz);
		}

		static void WorldToPositionCenter(int vx, int vy, int vz, float& x, float& y, float& z) {
			x = WorldToPositionCenter(vx);
			y = WorldToPositionCenter(vy);
			z = WorldToPositionCenter(vz);
		}

		static void PositionToWorld(float px, float py, float pz, int& wx, int& wy, int&wz) {
			wx = PositionToWorld(px);
			wy = PositionToWorld(py);
			wz = PositionToWorld(pz);
		}

		static void PositionToWorld(float px, float py, float pz, int& wx, int& wy, int&wz, float& rx, float& ry, float& rz) {
			wx = PositionToWorld(px, rx);
			wy = PositionToWorld(py, ry);
			wz = PositionToWorld(pz, rz);
		}

		static void GetSideNormal(const CoordI&, int side, float& nx, float& ny, float& nz) {
			float cNormalData[6][3] = {
				{0, 0, -1}, // 0
				{-1, 0, 0}, // 1
				{0, +1, 0}, // 2
				{+1, 0, 0}, // 3
				{0, -1, 0}, // 4
				{0, 0, +1}  // 5
			};
			nx = cNormalData[side][0];
			ny = cNormalData[side][1];
			nz = cNormalData[side][2];
		}

		static Vec3f CubeSidePoint(const CoordI&, unsigned int side_index, float h, float u, float v) {
			return BaseProperties::CubeSidePointBase(side_index, h, u, v);
		}
	};

	/** This constructs cubes using the topology of a cone */
	struct ConeProperties
	: public BaseProperties
	{
		/** Number of cells around the cone perimeter */
//		static const int cPerimeterCellCount = 16;
//		static const float cRadius = 40.7416201033f; //1.0f / (2.0f * std::tan(3.14159265359f / float(cPerimeterCellCount * CellSize)));

		static const int cPerimeterCellCount = 64;
		static const float cRadius = 162.974150399f; //1.0f / (2.0f * std::tan(3.14159265359f / float(cPerimeterCellCount * CellSize)));

		static const int cPerimeterCubeCount = cPerimeterCellCount * CellSize;

	private:
		static inline unsigned int WrappedWorld(int w) {
			return SignedMaskMod<cPerimeterCubeCount - 1>(w);
		}

		static inline unsigned int WorldToLocal(int w) {
			return SignedMaskMod<CellSizeMask>(w);
		}

		static inline int WorldToCellWrapped(int w) {
			return SignedShiftDiv<CellSizeShift>(int(WrappedWorld(w)));
		}

		static inline int WorldToCell(int w) {
			return SignedShiftDiv<CellSizeShift>(w);
		}

		static inline int CellLocalToWorldWrapped(int cell, unsigned int local) {
			unsigned int cell_unique = SignedMaskMod<cPerimeterCellCount - 1>(cell);
			 return int(CellSize * cell_unique + local);
		}

		static inline int CellLocalToWorld(int cell, unsigned int local) {
			 return int(CellSize) * cell + int(local);
		}

		static const float c2Pi = 6.28318530718f;
		static const float cPi = 3.14159265359f;

		static inline float PerimeterAngle(int x) {
			unsigned int w = WrappedWorld(x);
			float phi = float(w) / float(cPerimeterCubeCount) * c2Pi;
			return phi;
		}

		static unsigned int PerimeterAngleToCube(float phi, float& rest) {
			assert(0 <= phi && phi <= c2Pi);
			float w = phi / c2Pi * float(cPerimeterCubeCount);
			float wi_floored = std::floor(w);
			rest = w - wi_floored;
			return (unsigned int)(wi_floored);
		}

	public:
		static inline void WorldToLocal(int wx, int wy, int wz, unsigned int& lx, unsigned int& ly, unsigned int& lz) {
			lx = WorldToLocal(wx);
			ly = WorldToLocal(wy);
			lz = WorldToLocal(wz);
		}

		static inline void WorldToCell(int wx, int wy, int wz, int& cx, int& cy, int& cz) {
			cx = WorldToCellWrapped(wx);
			cy = WorldToCell(wy);
			cz = WorldToCell(wz);
		}

		static inline void CellLocalToWorld(int cx, int cy, int cz, unsigned int lx, unsigned int ly, unsigned int lz, int& wx, int& wy, int& wz) {
			wx = CellLocalToWorldWrapped(cx, lx);
			wy = CellLocalToWorld(cy, ly);
			wz = CellLocalToWorld(cz, lz);
		}

		static inline void WorldToPosition(int vx, int vy, int vz, float& x, float& y, float& z) {
			float phi = PerimeterAngle(vx);
			float r = cRadius + float(vz);
			x = r * std::sin(phi);
			y = float(vy);
			z = r * std::cos(phi);
		}

		static inline void WorldToPositionCenter(int vx, int vy, int vz, float& x, float& y, float& z) {
			float phi = PerimeterAngle(vx) + 0.5f / float(cPerimeterCubeCount) * c2Pi;
			float r = cRadius + float(vz) + 0.5f;
			x = r * std::sin(phi);
			y = float(vy) + 0.5f;
			z = r * std::cos(phi);
		}

		static inline void PositionToWorld(float px, float py, float pz, int& wx, int& wy, int& wz) {
			// convert (px,pz) to polar coordinates
			float r = std::sqrt(px*px + pz*pz);
			float phi = std::atan2(px, pz);
			if(phi < 0) {
				phi += c2Pi;
			}
			float w_rest;
			wx = int(PerimeterAngleToCube(phi, w_rest));
			wy = int(std::floor(py));
			wz = int(std::floor(r - cRadius));
		}

		static inline void PositionToWorld(float px, float py, float pz, int& wx, int& wy, int& wz, float& rx, float& ry, float& rz) {
			// convert (px,pz) to polar coordinates
			float r = std::sqrt(px*px + pz*pz);
			float phi = std::atan2(px, pz);
			if(phi < 0) {
				phi += c2Pi;
			}
			wx = int(PerimeterAngleToCube(phi, rx));
			float wy_floored = std::floor(py);
			wy = int(wy_floored);
			ry = py - wy_floored;
			float wz_floored = std::floor(r - cRadius);
			wz = int(wz_floored);
			rz = pz - wz_floored;
		}

		static inline void GetSideNormal(const CoordI& cw, int side, float& nx, float& ny, float& nz) {
			float cNormalData[6][3] = {
				{0, 0, -1}, // 0
				{-1, 0, 0}, // 1
				{0, +1, 0}, // 2
				{+1, 0, 0}, // 3
				{0, -1, 0}, // 4
				{0, 0, +1}  // 5
			};
			float nx_0 = cNormalData[side][0];
			float ny_0 = cNormalData[side][1];
			float nz_0 = cNormalData[side][2];
			// now rotate the side normal by the perimeter angle around the y axis
			float phi = PerimeterAngle(cw.x);
			float cp = std::cos(phi);
			float sp = std::sin(phi);
			nx = cp * nx_0 + sp * nz_0;
			ny = ny_0;
			nz = - sp * nx_0 + cp * nz_0;
			// FIXME optimize!
		}

		static inline Vec3f CubeSidePoint(const CoordI& c_world, unsigned int side_index, float h, float u, float v) {
			Vec3f a = BaseProperties::CubeSidePointBase(side_index, h, u, v);
			float phi = PerimeterAngle(c_world.x);
			Eigen::Affine3f t(Eigen::AngleAxisf(phi, Eigen::Vector3f(0, 1, 0)));
			return t * a;
		}

	};

	/** Sphere topology for cubes */
	struct SphereProperties
	: public BaseProperties
	{
		/** Number of cells around the cone perimeter */
//		static const int cPerimeterCellCount = 16;
//		static const float cRadius = 40.7416201033f; //1.0f / (2.0f * std::tan(3.14159265359f / float(cPerimeterCellCount * CellSize)));

		static const unsigned int cPerimeterCellCount = 64;
		static const unsigned int cHalfPerimeterCellCount = cPerimeterCellCount / 2;
		static const float cRadius = 162.974150399f; //1.0f / (2.0f * std::tan(3.14159265359f / float(cPerimeterCellCount * CellSize)));

		static const unsigned int cPerimeterCubeCount = cPerimeterCellCount * CellSize;
		static const unsigned int cHalfPerimeterCubeCount = cHalfPerimeterCellCount * CellSize;

	private:
		/** Wraps to [0, perimeter_cube_count[ */
		static inline unsigned int WrappedWorldX(int w) {
			return SignedMaskMod<cPerimeterCubeCount - 1>(w);
		}

		/** Wraps to [-perimeter_cube_count/2, +perimeter_cube_count/2[ */
		static inline int WrappedWorldY(int w) {
			unsigned int x = SignedMaskMod<cPerimeterCubeCount - 1>(w);
			if(x >= cHalfPerimeterCubeCount) {
				return int(x) - int(cPerimeterCubeCount);
			}
			else {
				return int(x);
			}
		}

		static inline unsigned int WorldToLocal(int w) {
			return SignedMaskMod<CellSizeMask>(w);
		}

		static inline int WorldToCellWrappedX(int w) {
			return SignedShiftDiv<CellSizeShift>(int(WrappedWorldX(w)));
		}

		static inline int WorldToCellWrappedY(int w) {
			return SignedShiftDiv<CellSizeShift>(WrappedWorldY(w));
		}

		static inline int WorldToCell(int w) {
			return SignedShiftDiv<CellSizeShift>(w);
		}

		static inline int CellLocalToWorldWrappedX(int cell, unsigned int local) {
			unsigned int cell_unique = SignedMaskMod<cPerimeterCellCount - 1>(cell);
			return int(CellSize * cell_unique + local);
		}

		static inline int CellLocalToWorldWrappedY(int cell, unsigned int local) {
			int cell_unique = int(SignedMaskMod<cPerimeterCellCount - 1>(cell));
			if(cell_unique >= int(cHalfPerimeterCellCount)) {
				cell_unique -= int(cPerimeterCellCount);
			}
			return int(CellSize) * cell_unique + int(local);
		}

		static inline int CellLocalToWorld(int cell, unsigned int local) {
			 return int(CellSize) * cell + int(local);
		}

		static const float c2Pi = 6.28318530718f;

		static inline float PerimeterAngleX(int x) {
			unsigned int w = WrappedWorldX(x);
			float phi = float(w) / float(cPerimeterCubeCount) * c2Pi;
			return phi;
		}

		static inline float PerimeterAngleY(int y) {
			int w = WrappedWorldY(y);
			float phi = float(w) / float(cPerimeterCubeCount) * c2Pi;
			return phi;
		}

		static inline unsigned int PerimeterAngleToCubeX(float phi, float& rest) {
			assert(0 <= phi && phi <= c2Pi);
			float w = phi / c2Pi * float(cPerimeterCubeCount);
			float wi_floored = std::floor(w);
			rest = w - wi_floored;
			return (unsigned int)(wi_floored);
		}

		static inline int PerimeterAngleToCubeY(float phi, float& rest) {
			//static const float cPi = 3.14159265359f;
			assert(-3.14159265359f <= phi && phi <= +3.14159265359f);
			float w = phi / c2Pi * float(cPerimeterCubeCount);
			float wi_floored = std::floor(w);
			rest = w - wi_floored;
			return int(wi_floored);
		}

	public:
		static void WorldToLocal(int wx, int wy, int wz, unsigned int& lx, unsigned int& ly, unsigned int& lz) {
			lx = WorldToLocal(wx);
			ly = WorldToLocal(wy);
			lz = WorldToLocal(wz);
		}

		static void WorldToCell(int wx, int wy, int wz, int& cx, int& cy, int& cz) {
			cx = WorldToCellWrappedX(wx);
			cy = WorldToCellWrappedY(wy);
			cz = WorldToCell(wz);
		}

		static void CellLocalToWorld(int cx, int cy, int cz, unsigned int lx, unsigned int ly, unsigned int lz, int& wx, int& wy, int& wz) {
			wx = CellLocalToWorldWrappedX(cx, lx);
			wy = CellLocalToWorldWrappedY(cy, ly);
			wz = CellLocalToWorld(cz, lz);
		}

		static void WorldToPosition(int vx, int vy, int vz, float& x, float& y, float& z) {
			float phix = PerimeterAngleX(vx);
			float phiy = PerimeterAngleY(vy);
			float r = cRadius + float(vz);
			x = r * std::cos(phiy) * std::sin(phix);
			y = r * std::sin(phiy);
			z = r * std::cos(phiy) * std::cos(phix);
		}

		static void WorldToPositionCenter(int vx, int vy, int vz, float& x, float& y, float& z) {
			float phix = PerimeterAngleX(vx) + 0.5f / float(cPerimeterCubeCount) * c2Pi;
			float phiy = PerimeterAngleX(vy) + 0.5f / float(cPerimeterCubeCount) * c2Pi;
			float r = cRadius + float(vz) + 0.5f;
			x = r * std::cos(phiy) * std::sin(phix);
			y = r * std::sin(phiy);
			z = r * std::cos(phiy) * std::cos(phix);
		}

		static void PositionToWorld(float px, float py, float pz, int& wx, int& wy, int& wz) {
			// convert (px,pz) to polar coordinates
			float r = std::sqrt(px*px + py*py + pz*pz);
			float phix = std::atan2(px, pz);
			if(phix < 0) {
				phix += c2Pi;
			}
			float phiy = std::asin(py / r);
			float w_rest;
			wx = int(PerimeterAngleToCubeX(phix, w_rest));
			wy = PerimeterAngleToCubeY(phiy, w_rest);
			wz = int(std::floor(r - cRadius));
		}

		static void PositionToWorld(float px, float py, float pz, int& wx, int& wy, int& wz, float& rx, float& ry, float& rz) {
			// convert (px,pz) to polar coordinates
			float r = std::sqrt(px*px + py*py + pz*pz);
			float phix = std::atan2(px, pz);
			if(phix < 0) {
				phix += c2Pi;
			}
			float phiy = std::asin(py / r);
			wx = int(PerimeterAngleToCubeX(phix, rx));
			wy = PerimeterAngleToCubeY(phiy, ry);
			float wz_floored = std::floor(r - cRadius);
			wz = int(wz_floored);
			rz = pz - wz_floored;
		}

		static void GetSideNormal(const CoordI& c_world, int side, float& nx, float& ny, float& nz) {
			float cNormalData[6][3] = {
				{0, 0, -1}, // 0
				{-1, 0, 0}, // 1
				{0, +1, 0}, // 2
				{+1, 0, 0}, // 3
				{0, -1, 0}, // 4
				{0, 0, +1}  // 5
			};
			Vec3f n0(cNormalData[side][0], cNormalData[side][1], cNormalData[side][2]);
			float phix = PerimeterAngleX(c_world.x);
			float phiy = PerimeterAngleY(c_world.y);
			Eigen::Affine3f tx(Eigen::AngleAxisf(phix, Eigen::Vector3f(0, 1, 0)));
			Eigen::Affine3f ty(Eigen::AngleAxisf(phiy, Eigen::Vector3f(1, 0, 0)));
			Vec3f n = ty * tx * n0;
			nx = n[0];
			ny = n[1];
			nz = n[2];
		}

		static Vec3f CubeSidePoint(const CoordI& c_world, unsigned int side_index, float h, float u, float v) {
			Vec3f a = BaseProperties::CubeSidePointBase(side_index, h, u, v);
			float phix = PerimeterAngleX(c_world.x);
			float phiy = PerimeterAngleY(c_world.y);
			Eigen::Affine3f tx(Eigen::AngleAxisf(phix, Eigen::Vector3f(0, 1, 0)));
			Eigen::Affine3f ty(Eigen::AngleAxisf(phiy, Eigen::Vector3f(1, 0, 0)));
			return ty * tx * a;
		}

	};

}

typedef CandyCubes::DerivedProperties<CandyCubes::SphereProperties> Properties;

namespace Exceptions
{
	struct InvalidCell {};
}

