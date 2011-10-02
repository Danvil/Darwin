#pragma once

#include "../Common.h"
#include <Candy/Tools/LinAlg.h>
#include <Candy/Tools/Math.h>

namespace Collision
{

	/// <summary>
	/// Intersects a 3D ray with a grid of axis aligned, evenly-spaced boxes of size 1
	/// For each cube encountered a test function is evaluated. If the test function
	/// returns true, the algorithm finishes returning true and giving the coordinates
	/// of the successfully tested box. If the ray leaves the grid without a successfull test,
	/// the return value is false.
	/// </summary>
	/// <param name="a">Starting point of the ray</param>
	/// <param name="u">Direction of the ray</param>
	/// <param name="min">Minimal box coordinate</param>
	/// <param name="max">Maximal box coordinate</param>
	/// <param name="test">Test function to evaluate for each box</param>
	/// <param name="result">The coordinates of the first ecountered box for which the test was successfull; or (0,0,0) if no test was successfull</param>
	/// <param name="side">The side index of the side which was hit</param>
	/// <returns>True, if a test was successfull; false, if the ray traversed all boxes an no test was successfull</returns>
	template<class Tester>
	bool GridRayIntersect(const Vec3f& a, const Vec3f& u, const Ci& min, const Ci& max, Tester& test, Ci& result, float& lambda, int& side) {
		using namespace Math;
		lambda = 0.0f;
		float ax = a(0);
		float ay = a(1);
		float az = a(2);
		int x, y, z;
		Properties::PositionToWorld(ax, ay, az, x, y, z);
		// if the starting cube is at the limit of the cell -> round to cell
		// FIXME is this correct?
		x = ClampHalfOpen(x, min.x, max.x);
		y = ClampHalfOpen(y, min.y, max.y);
		z = ClampHalfOpen(z, min.z, max.z);
		if(test(x, y, z, lambda)) {
			result.x = x;
			result.y = y;
			result.z = z;
			return true;
		}
		// prevent u_i from beeing 0!
		// FIXME this is a ugly workaround
		const float cDelta = 0.00001f;
		float ux = (u(0) == 0 ? cDelta : u(0));
		float uy = (u(1) == 0 ? cDelta : u(1));
		float uz = (u(2) == 0 ? cDelta : u(2));
		// direction in which we will move through the cubes
		int dx = Sign(ux);
		int dy = Sign(uy);
		int dz = Sign(uz);
		// variables used to compute in which direction (x/y/z) our ray
		// will penetrate a axis aligned plane next
		// planes have the (fixed) distance 1
		float lx = (float(x + (dx > 0 ? 1 : 0)) - ax) / ux;
		float ly = (float(y + (dy > 0 ? 1 : 0)) - ay) / uy;
		float lz = (float(z + (dz > 0 ? 1 : 0)) - az) / uz;
		float dlx = float(dx) / ux;
		float dly = float(dy) / uy;
		float dlz = float(dz) / uz;
		// follow the ray while it traverses the cell
		while(true) {
			// tests to determine in which direction the ray penetrates next
			bool cxy = lx <= ly;
			bool cxz = lx <= lz;
			bool cyz = ly <= lz;
			if(cxy && cxz) {
				// x is next
				x += dx;
				lambda = lx;
				side = Properties::SideIndexX(-dx); // if we move to the right we hit the left side first!
				if(!IsInHalfOpenInterval(x, min.x, max.x)) {
					break; // quit
				}
				lx += dlx;
			}
			else if(!cxy && cyz) {
				// y is next
				y += dy;
				lambda = ly;
				side = Properties::SideIndexY(-dy);
				if(!IsInHalfOpenInterval(y, min.y, max.y)) {
					break; // quit
				}
				ly += dly;
			}
			else /*if(!cxz && !cyz)*/ {
				// z is next
				z += dz;
				lambda = lz;
				side = Properties::SideIndexZ(-dz);
				if(!IsInHalfOpenInterval(z, min.z, max.z)) {
					break; // quit
				}
				lz += dlz;
			}
			// check the new cube and possibly quit
			if(test(x, y, z, lambda)) {
				result.x = x;
				result.y = y;
				result.z = z;
				return true;
			}
		}
		// outside of limit
		result.x = 0;
		result.y = 0;
		result.z = 0;
		return false;
	}

	template<class Tester>
	bool GridRayIntersect(const Vec3f& a, const Vec3f& u, Tester& test, Ci& result, float& lambda, int& side) {
		using namespace Math;
		lambda = 0.0f;
		float ax = a(0);
		float ay = a(1);
		float az = a(2);
		// get initial position to check
		Ci pos = Properties::PositionToWorld(a);
		// FIXME check initial position??
		//if(test(pos)) {
		//	result = pos;
		//	return true;
		//}
		// prevent u_i from beeing 0!
		// TODO this is a ugly workaround
		const float cDelta = 0.00001f;
		float ux = (u(0) == 0 ? cDelta : u(0));
		float uy = (u(1) == 0 ? cDelta : u(1));
		float uz = (u(2) == 0 ? cDelta : u(2));
		// direction in which we will move through the cubes
		int dx = Sign(ux);
		int dy = Sign(uy);
		int dz = Sign(uz);
		// variables used to compute in which direction (x/y/z) our ray
		// will penetrate a axis aligned plane next
		// planes have the (fixed) distance 1
		float lx = (float(pos.x + (dx > 0 ? 1 : 0)) - ax) / ux;
		float ly = (float(pos.y + (dy > 0 ? 1 : 0)) - ay) / uy;
		float lz = (float(pos.z + (dz > 0 ? 1 : 0)) - az) / uz;
		float dlx = float(dx) / ux;
		float dly = float(dy) / uy;
		float dlz = float(dz) / uz;
		// keep track of the last movement we did; 0:x, 1:y, 2:z
		int last_move = -1; 
		// follow the ray while it traverses the cell
		while(true) {
			// tests to determine in which direction the ray penetrates next
			bool cxy = lx <= ly;
			bool cxz = lx <= lz;
			bool cyz = ly <= lz;
			if(cxy && cxz) {
				// x is next
				pos.x += dx;
				lx += dlx;
				last_move = 0;
			}
			else if(!cxy && cyz) {
				// y is next
				pos.y += dy;
				ly += dly;
				last_move = 1;
			}
			else /*if(!cxz && !cyz)*/ {
				// z is next
				pos.z += dz;
				lz += dlz;
				last_move = 2;
			}
			// check the new cube and possibly quit
			if(test(pos)) {
				// this is done only once!
				if(last_move == 0) {
					lambda = lx - dlx; // get the lambda value before we entered the position
					side = Properties::SideIndexX(-dx); // if we move to the right we hit the left side first!
				} else if(last_move == 1) {
					lambda = ly - dly;
					side = Properties::SideIndexY(-dy);
				} else /* if(last_move == 2)*/ {
					lambda = lz - dlz;
					side = Properties::SideIndexZ(-dz);
				}
				result = pos;
				return true;
			}
		}
		// never gets here
	}

	/*template<class Tester>
	bool GridRayIntersectIncrementCheck(const Vec3f& a, const Vec3f& u, Tester& test, Ci& result, float& lambda, int& side) {
		using namespace Math;
		lambda = 0.0f;
		// get initial position to check
		Ci pos = Properties::PositionToInt(a);
		// FIXME check initial position??
		//if(test(pos)) {
		//	result = pos;
		//	return true;
		//}
		// prevent u_i from beeing 0!
		// TODO this is a ugly workaround
		const float cDelta = 0.00001f;
		float ux = (u.x == 0 ? cDelta : u.x);
		float uy = (u.y == 0 ? cDelta : u.y);
		float uz = (u.z == 0 ? cDelta : u.z);
		// direction in which we will move through the cubes
		int dx = Sign(ux);
		int dy = Sign(uy);
		int dz = Sign(uz);
		// variables used to compute in which direction (x/y/z) our ray
		// will penetrate a axis aligned plane next
		// planes have the (fixed) distance 1
		float lx = (float(pos.x + (dx > 0 ? 1 : 0)) - a.x) / ux;
		float ly = (float(pos.y + (dy > 0 ? 1 : 0)) - a.y) / uy;
		float lz = (float(pos.z + (dz > 0 ? 1 : 0)) - a.z) / uz;
		float dlx = float(dx) / ux;
		float dly = float(dy) / uy;
		float dlz = float(dz) / uz;
		// keep track of the last movement we did; 0:x, 1:y, 2:z
		int last_move = -1;
		test(pos, last_move, 0); // initialize
		// follow the ray while it traverses the cell
		while(true) {
			// tests to determine in which direction the ray penetrates next
			bool cxy = lx <= ly;
			bool cxz = lx <= lz;
			bool cyz = ly <= lz;
			int dir = 0;
			if(cxy && cxz) {
				// x is next
				pos.x += dx;
				lx += dlx;
				last_move = 0;
				dir = dx;
			}
			else if(!cxy && cyz) {
				// y is next
				pos.y += dy;
				ly += dly;
				last_move = 1;
				dir = dy;
			}
			else { //if(!cxz && !cyz)
				// z is next
				pos.z += dz;
				lz += dlz;
				last_move = 2;
				dir = dz;
			}
			// check the new cube and possibly quit
			if(test(pos, last_move, dir)) {
				// this is done only once!
				if(last_move == 0) {
					lambda = lx - dlx; // get the lambda value before we entered the position
					side = Properties::SideIndexX(-dx); // if we move to the right we hit the left side first!
				} else if(last_move == 1) {
					lambda = ly - dly;
					side = Properties::SideIndexY(-dy);
				} else { //if(last_move == 2)
					lambda = lz - dlz;
					side = Properties::SideIndexZ(-dz);
				}
				result = pos;
				return true;
			}
		}
		// never gets here
	}*/

}
