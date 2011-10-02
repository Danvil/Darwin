/*
 * Radiosity.cpp
 *
 *  Created on: 11.08.2011
 *      Author: david
 */

#include "Radiosity.hpp"
#include "LightingHelpers.hpp"
#include "../Cubes.h"
#include "../Appearance.h"
#include <boost/progress.hpp>
#include <boost/thread.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <iostream>

namespace Hexa
{
	namespace Lighting
	{
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
					float col_r = B_r_(i);
					float col_g = B_g_(i);
					float col_b = B_b_(i);
					CubeSideLightData& light = cit.data()->lighting;
					light.scenery.x() = col_r;
					light.scenery.y() = col_g;
					light.scenery.z() = col_b;
					light.scenery_with_object = cit.data()->object_color.cwiseProduct(light.scenery);
				}
				cubes->OnChangeCell(*it, false);
			}
		}

		bool ComputeFormFactor(const Ptr(Cubes)& cubes, Cell::BorderSideIterator& a, Cell::BorderSideIterator& b, float* ff)
		{
//			Vec3f a_mid, b_mid;
//			Vec3f a_n, b_n;
//			QuadMidAndNormal(a.side(), a_mid, a_n);
//			a_mid += a.positionCenter();
//			QuadMidAndNormal(b.side(), b_mid, b_n);
//			b_mid += b.positionCenter();
			// compute face midpoints
			Vec3f a_mid = a.positionCenter() + RandomCubeSidePoint(a.side());
			Vec3f b_mid = b.positionCenter() + RandomCubeSidePoint(b.side());
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
			float q = 1.0f / 3.1415f * std::abs(a_cos * b_cos) / (d_sq_norm * d_sq_norm);
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

		float PatchSelectionCriterion(float r, float g, float b) {
			// return r + g + b;
			return std::max(std::max(r,g),b);
		}

		void Radiosity::Prepare(const Ptr(Cubes)& cubes)
		{
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
					const Vec3f& emit_color = Appearance::CubeEmitColor(cit.type());
					E_r_(i) = emit_color(0);
					E_g_(i) = emit_color(1);
					E_b_(i) = emit_color(2);
					albedo_r_(i) = 0.70f * cit.data()->object_color(0);
					albedo_g_(i) = 0.70f * cit.data()->object_color(1);
					albedo_b_(i) = 0.70f * cit.data()->object_color(2);
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

		RadiosityShooting::RadiosityShooting()
		{
		}

		uint64_t RadiosityShooting::Iterate(Ptr(Cubes) cubes)
		{
			if(ff_computed_.empty()) {
				// initialize
				Prepare(cubes);
			}
			uint64_t n = ShootLight(cubes);
			if(n > 0) {
				Update(cubes);
//				static unsigned int s_i = 0;
//				std::cout << "Iteration " << ++s_i << std::endl;
			}
			return n;
		}

		void RadiosityShooting::Prepare(const Ptr(Cubes)& cubes)
		{
			// get border sides
			for(CellIterator cit=cubes->IterateCells(); cit; ++cit) {
				cells_.push_back(cit);
				const LinearKeyValueContainer* cell_sides = cit->GetBorderSides();
				quads_count_ += cell_sides ? cell_sides->Count() : 0;
			}
			// now form factors yet
			ff_computed_.resize(quads_count_, 0);
			ff_ = Eigen::DynamicSparseMatrix<float,Eigen::RowMajor>(quads_count_, quads_count_);
			// compute emittance and albedo
			E_r_ = Eigen::VectorXf(quads_count_);
			E_g_ = Eigen::VectorXf(quads_count_);
			E_b_ = Eigen::VectorXf(quads_count_);
			albedo_r_ = Eigen::VectorXf(quads_count_);
			albedo_g_ = Eigen::VectorXf(quads_count_);
			albedo_b_ = Eigen::VectorXf(quads_count_);
			size_t i=0;
			for(auto it=cells_.cbegin(); it!=cells_.cend(); ++it) {
				for(Cell::BorderSideIterator cit=(*it)->IterateBorderSides(); cit; ++cit, i++) {
					const Vec3f& emit_color = Appearance::CubeEmitColor(cit.type());
					E_r_(i) = emit_color(0);
					E_g_(i) = emit_color(1);
					E_b_(i) = emit_color(2);
					float ar = 0.70f * cit.data()->object_color(0);
					float ag = 0.70f * cit.data()->object_color(1);
					float ab = 0.70f * cit.data()->object_color(2);
					albedo_r_(i) = ar;
					albedo_g_(i) = ag;
					albedo_b_(i) = ab;
					ambient_scl_r_ += ar;
					ambient_scl_g_ += ag;
					ambient_scl_b_ += ab;
				}
			}
			ambient_scl_r_ = 1 / (float(quads_count_) - ambient_scl_r_);
			ambient_scl_g_ = 1 / (float(quads_count_) - ambient_scl_g_);
			ambient_scl_b_ = 1 / (float(quads_count_) - ambient_scl_b_);
			// initial guess for B
			B_r_ = E_r_;
			B_g_ = E_g_;
			B_b_ = E_b_;
			// no light distributed yet
			B_unshot_r_ = B_r_;
			B_unshot_g_ = B_g_;
			B_unshot_b_ = B_b_;
		}

		uint64_t RadiosityShooting::ShootLight(Ptr(Cubes) cubes)
		{
			const unsigned int cThreadCount = 4;
			if(cThreadCount == 1) {
				if(!ShootLightST(cubes)) {
					return 0;
				}
			}
			else {
				if(!ShootLightMT(cubes, cThreadCount)) {
					return 0;
				}
			}
			return cThreadCount * quads_count_;
		}

		bool RadiosityShooting::ShootLightST(Ptr(Cubes) cubes)
		{
			// find patch with highest value of undistributed radiosity
			float total_unshot_r = 0.0f;
			float total_unshot_g = 0.0f;
			float total_unshot_b = 0.0f;
			size_t selected_patch = 0;
			float max_rad = B_unshot_r_(0) + B_unshot_g_(0) + B_unshot_b_(0);
			for(size_t i=0; i<quads_count_; i++) {
				total_unshot_r += B_unshot_r_(i);
				total_unshot_g += B_unshot_g_(i);
				total_unshot_b += B_unshot_b_(i);
				float rad = PatchSelectionCriterion(B_unshot_r_(i), B_unshot_g_(i), B_unshot_b_(i));
				if(rad > max_rad) {
					selected_patch = i;
					max_rad = rad;
				}
			}
			if(max_rad == 0.0f) {
				return false;
			}
//			std::cout << "Unshot: " << total_unshot_r << "/" << total_unshot_g << "/" << total_unshot_b << std::endl;
			// compute form factors for patch (if necesary)
			if(!ff_computed_[selected_patch]) {
				ComputeFormFactorsForPatch(cubes, cells_, selected_patch, [&](size_t i, size_t j, float q) {
					ff_.insert(i,j) = q;
				});
				ff_computed_[selected_patch] = 1;
			}
			// shoot radiosity
			for(size_t j=0; j<quads_count_; j++) {
				float ff = ff_.coeff(selected_patch,j);
				float d_rad_r = albedo_r_(j) * B_unshot_r_(selected_patch) * ff;
				float d_rad_g = albedo_g_(j) * B_unshot_g_(selected_patch) * ff;
				float d_rad_b = albedo_b_(j) * B_unshot_b_(selected_patch) * ff;
				B_unshot_r_(j) += d_rad_r;
				B_unshot_g_(j) += d_rad_g;
				B_unshot_b_(j) += d_rad_b;
				B_r_(j) += d_rad_r;
				B_g_(j) += d_rad_g;
				B_b_(j) += d_rad_b;
			}
			ambient_r_ = ambient_scl_r_ * (total_unshot_r - B_unshot_r_(selected_patch));
			ambient_g_ = ambient_scl_g_ * (total_unshot_g - B_unshot_g_(selected_patch));
			ambient_b_ = ambient_scl_b_ * (total_unshot_b - B_unshot_b_(selected_patch));
			B_unshot_r_(selected_patch) = 0.0f;
			B_unshot_g_(selected_patch) = 0.0f;
			B_unshot_b_(selected_patch) = 0.0f;
			return true;
		}

		bool RadiosityShooting::ShootLightMT(Ptr(Cubes) cubes, unsigned int thread_count)
		{
			// prepare array for patch sorting and sum total undistributed light
			std::vector<size_t> patch_ids(quads_count_);
			float total_unshot_r = 0.0f;
			float total_unshot_g = 0.0f;
			float total_unshot_b = 0.0f;
			for(size_t i=0; i<quads_count_; i++) {
				patch_ids[i] = i;
				total_unshot_r += B_unshot_r_(i);
				total_unshot_g += B_unshot_g_(i);
				total_unshot_b += B_unshot_b_(i);
			}
			// break if all light has been distributed
			if(total_unshot_r + total_unshot_g + total_unshot_b == 0.0f) {
				return false;
			}
			// find patches with maximum amount of undistributed light
			std::partial_sort(patch_ids.begin(), patch_ids.begin() + thread_count, patch_ids.end(), [&](size_t a, size_t b) {
				// use sum of channel light for comparison
				float rad_a = PatchSelectionCriterion(B_unshot_r_(a), B_unshot_g_(a), B_unshot_b_(a));
				float rad_b = PatchSelectionCriterion(B_unshot_r_(b), B_unshot_g_(b), B_unshot_b_(b));
				return rad_a > rad_b;
			});
			patch_ids.resize(thread_count);
			//std::cout << PatchSelectionCriterion(B_unshot_r_(patch_ids[0]), B_unshot_g_(patch_ids[0]), B_unshot_b_(patch_ids[0])) << std::endl;
			// compute form factors for selected patches, one thread per patch
			std::vector<boost::thread*> threads;
			boost::mutex m;
			for(size_t i=0; i<thread_count; i++) {
				// create and run threads
				threads.push_back(new boost::thread([&](size_t selected_patch) {
					//std::cout << "Selected: " << selected_patch << std::endl;
					if(!ff_computed_[selected_patch]) {
						ComputeFormFactorsForPatch(cubes, cells_, selected_patch, [&](size_t i, size_t j, float q) {
							boost::interprocess::scoped_lock<boost::mutex> l(m);
							//std::cout << "(" << i << "," << j << ")=" << q << std::endl;
							ff_.insert(i,j) = q;
						});
						ff_computed_[selected_patch] = 1;
					}
				}, patch_ids[i]));
			}
			// wait for form factor computation to complete
			for(size_t i=0; i<threads.size(); i++) {
				//std::cout << patch_ids[i] << " " << std::flush;
				threads[i]->join();
				delete threads[i];
			}
			// distribute light from selected patches to all other patches
			for(size_t j=0; j<quads_count_; j++) {
				float d_rad_r = 0.0f;
				float d_rad_g = 0.0f;
				float d_rad_b = 0.0f;
				for(size_t i=0; i<thread_count; i++) {
					size_t selected_patch = patch_ids[i];
					float form_factor = ff_.coeff(selected_patch,j);
					d_rad_r += B_unshot_r_(selected_patch) * form_factor;
					d_rad_g += B_unshot_g_(selected_patch) * form_factor;
					d_rad_b += B_unshot_b_(selected_patch) * form_factor;
				}
				d_rad_r *= albedo_r_(j);
				d_rad_g *= albedo_g_(j);
				d_rad_b *= albedo_b_(j);
				B_unshot_r_(j) += d_rad_r;
				B_unshot_g_(j) += d_rad_g;
				B_unshot_b_(j) += d_rad_b;
				B_r_(j) += d_rad_r;
				B_g_(j) += d_rad_g;
				B_b_(j) += d_rad_b;
			}
			// sum up distributed light and set value for undistributed light to 0 for processed patches
			float shot_r = 0.0f;
			float shot_g = 0.0f;
			float shot_b = 0.0f;
			for(size_t i=0; i<thread_count; i++) {
				size_t selected_patch = patch_ids[i];
				shot_r += B_unshot_r_(selected_patch);
				shot_g += B_unshot_g_(selected_patch);
				shot_b += B_unshot_b_(selected_patch);
				B_unshot_r_(selected_patch) = 0.0f;
				B_unshot_g_(selected_patch) = 0.0f;
				B_unshot_b_(selected_patch) = 0.0f;
			}
			//std::cout << "Distributed " << shot_r << "/" << shot_g << "/" << shot_b << std::endl;
			ambient_r_ = ambient_scl_r_ * (total_unshot_r - shot_r);
			ambient_g_ = ambient_scl_g_ * (total_unshot_g - shot_g);
			ambient_b_ = ambient_scl_b_ * (total_unshot_b - shot_b);
			return true;
		}

		void RadiosityShooting::Update(const Ptr(Cubes)& cubes)
		{
			size_t i=0;
			for(auto it=cells_.cbegin(); it!=cells_.cend(); ++it) {
				for(Cell::BorderSideIterator cit=(*it)->IterateBorderSides(); cit; ++cit, i++) {
					float col_r = B_r_(i);
					float col_g = B_g_(i);
					float col_b = B_b_(i);
					CubeSideLightData& light = cit.data()->lighting;
					light.scenery.x() = col_r + albedo_r_(i) * ambient_r_;
					light.scenery.y() = col_g + albedo_g_(i) * ambient_g_;
					light.scenery.z() = col_b + albedo_b_(i) * ambient_b_;
					light.scenery_with_object = cit.data()->object_color.cwiseProduct(light.scenery);
				}
				cubes->OnChangeCell(*it, false);
			}
		}

	}
}
