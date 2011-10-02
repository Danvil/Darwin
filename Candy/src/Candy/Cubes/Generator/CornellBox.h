/*
 * CornellBox.h
 *
 *  Created on: May 2, 2011
 *      Author: david
 */

#ifndef CORNELLBOX_H_
#define CORNELLBOX_H_

namespace Generators
{
	/**
	 * Preferred size: x1=-4, x2=+4, y1=-4, y2=+4, z1=-4, z2=+4;
	 */
	struct CornellBox
	{
	private:
		int cBoxSize;
		int cLightSize;
		double cSphereRadius;
		int cSphereZOffset;

	public:
		CornellBox(unsigned int r=4) {
			cBoxSize = Common::CellSize * r - 2;
			cLightSize = Common::CellSize * r / 2;
			cSphereRadius = 0.70 * double(Common::CellSize * r);
			cSphereZOffset = cBoxSize - std::floor(cSphereRadius);
		}

		CubeType operator()(const CoordI& cw) {
			const CubeType t_sides = CubeTypes::DebugWhite;
			const CubeType t_sides_red = CubeTypes::DebugRed;
			const CubeType t_sides_green = CubeTypes::DebugGreen;
			const CubeType t_sides_blue = CubeTypes::DebugBlue;
			const CubeType t_sphere = CubeTypes::DebugWhite;
			const CubeType t_light = CubeTypes::LightWhite;
			int x = cw.x;
			int y = cw.y;
			int z = cw.z;
			// back side
			if(y == +cBoxSize && -cBoxSize < x && x < cBoxSize && -cBoxSize < z && z < cBoxSize) {
				return t_sides;
			}
			// ceiling
			if(z == cBoxSize - 1 && -cBoxSize < x && x < cBoxSize && -cBoxSize < y && y < cBoxSize) {
				if(-cLightSize < x && x < cLightSize && -cLightSize < y && y < cLightSize) {
					return t_light;
				}
				else {
					return t_sides;
				}
			}
			// doubled layered ceiling to avoid unnecessary light loss to the top
			if(z == cBoxSize && -cBoxSize < x && x < cBoxSize && -cBoxSize < y && y < cBoxSize) {
				return t_sides;
			}
			// bottom side
			if(z == -cBoxSize && -cBoxSize < x && x < cBoxSize && -cBoxSize < y && y < cBoxSize) {
				return t_sides_blue;
			}
			// left side
			if(x == -cBoxSize && -cBoxSize < y && y < cBoxSize && -cBoxSize < z && z < cBoxSize) {
				return t_sides_red;
			}
			// right side
			if(x == +cBoxSize && -cBoxSize < y && y < cBoxSize && -cBoxSize < z && z < cBoxSize) {
				return t_sides_green;
			}
			// sphere
			Vec3f pos = Common::WorldToPositionCenter(x, y, z + cSphereZOffset);
			if(pos.norm() < cSphereRadius) {
				return t_sphere;
			}
			return CubeTypes::Empty;
		}
	};
}

#endif
