/*
 * RadiosityHelper.hpp
 *
 *  Created on: Oct 13, 2011
 *      Author: david
 */

#ifndef RADIOSITYHELPER_HPP_
#define RADIOSITYHELPER_HPP_

#include "../Cubes.h"
#include "LightingHelpers.hpp"

namespace Hexa { namespace Lighting {

inline bool ComputeFormFactor(const Ptr(Cubes)& cubes, Cell::BorderSideIterator& a, Cell::BorderSideIterator& b, float* ff)
{
//			Vec3f a_mid, b_mid;
//			Vec3f a_n, b_n;
//			QuadMidAndNormal(a.side(), a_mid, a_n);
//			a_mid += a.positionCenter();
//			QuadMidAndNormal(b.side(), b_mid, b_n);
//			b_mid += b.positionCenter();
	// compute face midpoints
	Vec3f a_mid = a.positionCenter() + CenterPointSlightlyOutsideOfCubeSide(a.world(), a.side()); // RandomCubeSidePoint(a.world(), a.side());
	Vec3f b_mid = b.positionCenter() + CenterPointSlightlyOutsideOfCubeSide(b.world(), b.side()); // RandomCubeSidePoint(a.world(), b.side());
	// face normals
	Vec3f a_n = Properties::SideNormal(a.world(), a.side());
	Vec3f b_n = Properties::SideNormal(a.world(), b.side());
	// contribution if we would have a hit
	Vec3f d = b_mid - a_mid;
	float a_cos = a_n.dot(d);
	float b_cos = -b_n.dot(d);
	if(a_cos <= 0.0f || b_cos <= 0.0f) {
		return false;
	}
	// cast ray between midpoints
	float d_sq_norm = d.squaredNorm();
	float d_norm = std::sqrt(d_sq_norm);
	float q = 1.0f / 3.1415f * a_cos * b_cos / (d_sq_norm * d_sq_norm);
	// remark: need an additional /d_sq_norm, because we
	// need to normalize d for cos computation (twice)
	*ff = q;
	return !cubes->Pick(a_mid, (1.0f / d_norm) * d, d_norm);
}

template<typename Op>
void ComputeFormFactorsForPatch(const Ptr(Cubes)& cubes, const std::vector<Cell*>& cells_, size_t selected_patch, Op op ) {
	Cell::BorderSideIterator cit;
	size_t j=0;
	for(auto jt=cells_.cbegin(); jt!=cells_.cend() && j<=selected_patch; ++jt) {
		for(Cell::BorderSideIterator cjt=(*jt)->IterateBorderSides(); cjt && j<=selected_patch; ++cjt, j++) {
			if(j == selected_patch) {
				cit = cjt;
			}
		}
	}
	j=0;
	for(auto jt=cells_.cbegin(); jt!=cells_.cend(); ++jt) {
		for(Cell::BorderSideIterator cjt=(*jt)->IterateBorderSides(); cjt; ++cjt, j++) {
//					if(j == selected_quad) {
//						continue;
//					}
			float q;
			if(ComputeFormFactor(cubes, cit, cjt, &q)) {
				op(selected_patch, j, q);
			}
		}
	}
}

inline float PatchSelectionCriterion(float r, float g, float b) {
	// return r + g + b;
	return std::max(std::max(r,g),b);
}

}}

#endif
