/*
 * Radiosity.hpp
 *
 *  Created on: 11.08.2011
 *      Author: david
 */

#ifndef RADIOSITY_HPP_
#define RADIOSITY_HPP_

#include "../Lighting.hpp"
#define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET
#include <Eigen/Sparse>
#include <vector>

class Cell;

namespace Hexa { namespace Lighting {

/** Global illumination using the basic radiosity algorithm */
class Radiosity
: public GlobalIlluminator
{
public:
	Radiosity();

	uint64_t Iterate(Ptr(Cubes) cubes);

private:
	void Prepare(const Ptr(Cubes)& cubes);

	void Update(const Ptr(Cubes)& cubes);

private:
	bool has_ff_;
	std::vector<Cell*> cells_;
	unsigned int quads_count_;
	Eigen::DynamicSparseMatrix<float,Eigen::RowMajor> ff_;
	Eigen::VectorXf albedo_r_, albedo_g_, albedo_b_;
	Eigen::VectorXf E_r_, E_g_, E_b_;
	Eigen::VectorXf B_r_, B_g_, B_b_;
};

}}

#endif
