/*
 * Radiosity.cpp
 *
 *  Created on: 11.08.2011
 *      Author: david
 */

#include "Radiosity.hpp"
#include "RadiosityHelper.hpp"
#include "../Cubes.h"
#include "../Appearance.h"
#include <boost/progress.hpp>
#include <boost/thread.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <iostream>

namespace Hexa { namespace Lighting {

Radiosity::Radiosity()
{
	has_ff_ = false;
}

uint64_t Radiosity::Iterate(Ptr(Cubes) cubes)
{
	// direct and stupid approach
	if(!has_ff_) {
		has_ff_ = true;
		// compute form factors
		Prepare(cubes);
//				for(unsigned int i=0; i<1; i++) {
//					B_r_ = E_r_ + (ff_ * B_r_).cwiseProduct(albedo_r_);
//					B_g_ = E_g_ + (ff_ * B_g_).cwiseProduct(albedo_g_);
//					B_b_ = E_b_ + (ff_ * B_b_).cwiseProduct(albedo_b_);
//				}
		Update(cubes);
	}
	else {
		// solve equation
		B_r_ = E_r_ + (ff_ * B_r_).cwiseProduct(albedo_r_);
		B_g_ = E_g_ + (ff_ * B_g_).cwiseProduct(albedo_g_);
		B_b_ = E_b_ + (ff_ * B_b_).cwiseProduct(albedo_b_);
		Update(cubes);
	}
	return quads_count_*quads_count_;
}

void Radiosity::Update(const Ptr(Cubes)& cubes)
{
	size_t i=0;
	for(auto it=cells_.cbegin(); it!=cells_.cend(); ++it) {
		for(Cell::BorderSideIterator cit=(*it)->IterateBorderSides(); cit; ++cit, i++) {
			cit.data()->setCurrentLight(Vec3f(B_r_(i), B_g_(i), B_b_(i)), 1.0f); // FIXME weight?
		}
		(*it)->SetAppearanceDirtyFlag();
	}
}

void Radiosity::Prepare(const Ptr(Cubes)& cubes)
{
	quads_count_ = 0;
	// get border sides
	for(CellIterator cit=cubes->IterateCells(); cit; ++cit) {
		cells_.push_back(cit);
		const LinearKeyValueContainer* cell_sides = cit->GetBorderSides();
		quads_count_ += cell_sides ? cell_sides->Count() : 0;
	}
	// we compute all form factors and store them in a sparse matrix
	std::cout << "Radiosity: Computing form factors (" << quads_count_ << " quads)" << std::endl;
	ff_ = Eigen::DynamicSparseMatrix<float,Eigen::RowMajor>(quads_count_, quads_count_);
	E_r_ = Eigen::VectorXf(quads_count_);
	E_g_ = Eigen::VectorXf(quads_count_);
	E_b_ = Eigen::VectorXf(quads_count_);
	albedo_r_ = Eigen::VectorXf(quads_count_);
	albedo_g_ = Eigen::VectorXf(quads_count_);
	albedo_b_ = Eigen::VectorXf(quads_count_);
	boost::progress_display progress(quads_count_);
	size_t i=0;
	for(auto it=cells_.cbegin(); it!=cells_.cend(); ++it) {
		for(Cell::BorderSideIterator cit=(*it)->IterateBorderSides(); cit; ++cit, i++) {
			// emittance
			const CubeMaterialProperties* material = cit.data()->getMaterial();
			E_r_(i) = material->emit_(0);
			E_g_(i) = material->emit_(1);
			E_b_(i) = material->emit_(2);
			albedo_r_(i) = material->albedo_(0);
			albedo_g_(i) = material->albedo_(1);
			albedo_b_(i) = material->albedo_(2);
			// form factors
			size_t j=0;
			for(auto jt=cells_.cbegin(); jt!=cells_.cend(); ++jt) {
				for(Cell::BorderSideIterator cjt=(*jt)->IterateBorderSides(); cjt; ++cjt, j++) {
					float q;
					if(ComputeFormFactor(cubes, cit, cjt, &q)) {
						ff_.insertBack(i,j) = q;
					}
				}
			}
			++progress;
		}
	}
	 // initial guess
	B_r_ = E_r_;
	B_g_ = E_g_;
	B_b_ = E_b_;
	std::cout << "--- Finished" << std::endl;
}

}}
