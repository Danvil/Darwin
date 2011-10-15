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
#include <algorithm>

namespace Hexa { namespace Lighting {

struct Patch
{
	bool form_factors_computed_;
	CoordI c_world_;
	//CoordI c_cell_;
	unsigned int side_;
	//BorderSideId bsid_;
	Vec3f side_point_;
	Vec3f side_normal_;
	float albedo_r_, albedo_g_, albedo_b_;
	float E_r_, E_g_, E_b_;
	float B_r_, B_g_, B_b_;
	float B_unshot_r_, B_unshot_g_, B_unshot_b_;
};

template<typename K>
struct FormFactorConnection
{
	struct Connection {
		Connection() {}
		Connection(size_t i, size_t strength)
		: i_(i), strength_(strength) {}
		size_t i_;
		K strength_;
	};
	void resize(size_t patch_count) {
		data_.clear();
		data_.resize(patch_count);
	}
	K& operator()(size_t row, size_t col) {
		BOOST_ASSERT(row < data_.size());
		std::vector<Connection>& c = data_[row];
		auto it = std::lower_bound(c.begin(), c.end(), col, [](const Connection& a, size_t val) {
			return a.i_ < val;
		});
		if(it == c.end() || it->i_ != col) {
			size_t p = it - c.begin();
			c.insert(it, Connection(col, 0.0f));
			return c[p].strength_;
		}
		return it->strength_;
	}
	const K& operator()(size_t row, size_t col) const {
		BOOST_ASSERT(row < data_.size());
		const std::vector<Connection>& c = data_[row];
		auto it = std::lower_bound(c.begin(), c.end(), col, [](const Connection& a, size_t val) {
			return a.i_ < val;
		});
		if(it == c.end() || it->i_ != col) {
			throw std::runtime_error("No such element!");
		}
		return it->strength_;
	}
private:
	std::vector<std::vector<Connection>> data_;
};

inline bool ComputeFormFactor(const Ptr(Cubes)& cubes, const Patch& a, const Patch& b, float* ff)
{
	// contribution if we would have a hit
	Vec3f d = b.side_point_ - a.side_point_;
	float d_sq_norm = d.squaredNorm();
	if(d_sq_norm > 1630.0f) {
		// less than 1/512 light at this distance (at patch intensity of 10 and parallel patches)
		return false;
	}
	float a_cos = a.side_normal_.dot(d);
	float b_cos = -b.side_normal_.dot(d);
	if(a_cos <= 0.0f || b_cos <= 0.0f) {
		return false;
	}
	// compute light transportation factor
	float q = 1.0f / 3.1415f * a_cos * b_cos / (d_sq_norm * d_sq_norm);
	// remark: first /d_sq_norm, because we need to normalize d for cos computation twice
	// remark: second /d_sq_norm, to account for patch distance in the geometry term
	*ff = q;
	// cast ray between midpoints and check for collision
	float d_norm = std::sqrt(d_sq_norm);
	float d_norm_inv = 1.0f / d_norm;
	return !cubes->Pick(a.side_point_, d_norm_inv * d, d_norm);
}

template<typename Op>
void ComputeFormFactorsForPatch(const Ptr(Cubes)& cubes, const std::vector<Patch>& patches, size_t selected_patch_index, Op op) {
	const Patch& selected_patch = patches[selected_patch_index];
	size_t j=0;
	for(const Patch& pj : patches) {
		float q;
		if(ComputeFormFactor(cubes, selected_patch, pj, &q)) {
			op(selected_patch_index, j, q);
		}
		j++;
	}
}


/**  DEPRECATED  */
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
	float d_sq_norm = d.squaredNorm();
	if(d_sq_norm > 1630.0f) {
		// less than 1/512 light at this distance (at patch intensity of 10 and parallel patches)
		return false;
	}
	float a_cos = a_n.dot(d);
	float b_cos = -b_n.dot(d);
	if(a_cos <= 0.0f || b_cos <= 0.0f) {
		return false;
	}
	// cast ray between midpoints
	float d_norm = std::sqrt(d_sq_norm);
	float q = 1.0f / 3.1415f * a_cos * b_cos / (d_sq_norm * d_sq_norm);
	// remark: first /d_sq_norm, because we need to normalize d for cos computation twice
	// remark: second /d_sq_norm, to account for patch distance in the geometry term
	*ff = q;
	return !cubes->Pick(a_mid, (1.0f / d_norm) * d, d_norm);
}

/**  DEPRECATED  */
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
