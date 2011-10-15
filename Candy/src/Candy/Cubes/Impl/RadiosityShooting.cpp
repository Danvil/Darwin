/*
 * RadiosityShooting.cpp
 *
 *  Created on: 11.08.2011
 *      Author: david
 */

#include "RadiosityShooting.hpp"
#include "RadiosityHelper.hpp"
#include "ExecuteInThreads.hpp"
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
		RadiosityShooting::RadiosityShooting()
		{
			iteration_ = 0;
		}

		void RadiosityShooting::Restart()
		{
			iteration_ = 0;
		}

		uint64_t RadiosityShooting::Iterate(Ptr(Cubes) cubes)
		{
			if(iteration_ == 0) {
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
			quads_count_ = 0;
			cells_.clear();
			// get border sides
			for(CellIterator cit=cubes->IterateCells(); cit; ++cit) {
				cells_.push_back(cit);
				const LinearKeyValueContainer* cell_sides = cit->GetBorderSides();
				quads_count_ += cell_sides ? cell_sides->Count() : 0;
			}
			// now form factors yet
			patches_.resize(quads_count_);
			//ff_.resize(quads_count_);
			ff_ = Eigen::DynamicSparseMatrix<float,Eigen::RowMajor>(quads_count_, quads_count_);
			// compute emittance and albedo
//			E_r_ = Eigen::VectorXf(quads_count_);
//			E_g_ = Eigen::VectorXf(quads_count_);
//			E_b_ = Eigen::VectorXf(quads_count_);
//			albedo_r_ = Eigen::VectorXf(quads_count_);
//			albedo_g_ = Eigen::VectorXf(quads_count_);
//			albedo_b_ = Eigen::VectorXf(quads_count_);
			size_t i =0;
			for(auto it=cells_.cbegin(); it!=cells_.cend(); ++it) {
				for(Cell::BorderSideIterator cit=(*it)->IterateBorderSides(); cit; ++cit, i++) {
					const Vec3f& emit_color = Appearance::CubeEmitColor(cit.type());
					Patch& p = patches_[i];
					p.form_factors_computed_ = false;
					// patch coordinates
					p.c_world_ = cit.world();
					//p.c_cell_ = (*it)->coordinate();
					p.side_ = cit.side();
					//p.bsid_ = cit.id();
					p.side_point_ = cit.positionCenter() + CenterPointSlightlyOutsideOfCubeSide(p.c_world_, p.side_);
					p.side_normal_ = Properties::SideNormal(p.c_world_, p.side_);
					// compute albedo from object color
					float ar = 0.70f * cit.data()->getObjectColor()(0);
					float ag = 0.70f * cit.data()->getObjectColor()(1);
					float ab = 0.70f * cit.data()->getObjectColor()(2);
					p.albedo_r_ = ar;
					p.albedo_g_ = ag;
					p.albedo_b_ = ab;
					// ambient scale factor
					ambient_scl_r_ += ar;
					ambient_scl_g_ += ag;
					ambient_scl_b_ += ab;
					// get patch emit color
					float er = emit_color(0);
					float eg = emit_color(1);
					float eb = emit_color(2);
					p.E_r_ = er;
					p.E_g_ = eg;
					p.E_b_ = eb;
					// initial guess for B
					p.B_r_ = er;
					p.B_g_ = eg;
					p.B_b_ = eb;
					// no light distributed yet
					p.B_unshot_r_ = er;
					p.B_unshot_g_ = eg;
					p.B_unshot_b_ = eb;
				}
			}
			ambient_scl_r_ = 1 / (float(quads_count_) - ambient_scl_r_);
			ambient_scl_g_ = 1 / (float(quads_count_) - ambient_scl_g_);
			ambient_scl_b_ = 1 / (float(quads_count_) - ambient_scl_b_);
//			// initial guess for B
//			B_r_ = E_r_;
//			B_g_ = E_g_;
//			B_b_ = E_b_;
//			// no light distributed yet
//			B_unshot_r_ = B_r_;
//			B_unshot_g_ = B_g_;
//			B_unshot_b_ = B_b_;
		}

		uint64_t RadiosityShooting::ShootLight(Ptr(Cubes) cubes)
		{
			uint64_t n = 0;
			const unsigned int cThreadCount = 1;
			const unsigned int cCount = 12;
			for(unsigned int i=0; i<cCount; i++) {
				n++;
//				if(cThreadCount == 1) {
					if(!ShootLightST(cubes)) {
						break;
					}
//				}
//				else {
//					if(!ShootLightMT(cubes, cThreadCount)) {
//						return n;
//					}
//				}
			}
			for(Cell* cell : cells_) {
				cell->AddSamples(n);
			}
			iteration_ += n;
			return n * cThreadCount * quads_count_;
		}

		bool RadiosityShooting::ShootLightST(Ptr(Cubes) cubes)
		{
			// find patch with highest value of undistributed radiosity
			float total_unshot_r = 0.0f;
			float total_unshot_g = 0.0f;
			float total_unshot_b = 0.0f;
			size_t selected_patch_index = 0;
			float max_rad = PatchSelectionCriterion(patches_[0].B_unshot_r_ , patches_[0].B_unshot_g_, patches_[0].B_unshot_b_);
			for(size_t i=0; i<quads_count_; i++) {
				Patch& p = patches_[i];
				total_unshot_r += p.B_unshot_r_;
				total_unshot_g += p.B_unshot_g_;
				total_unshot_b += p.B_unshot_b_;
				float rad = PatchSelectionCriterion(p.B_unshot_r_, p.B_unshot_g_, p.B_unshot_b_);
				if(rad > max_rad) {
					selected_patch_index = i;
					max_rad = rad;
				}
			}
			if(max_rad == 0.0f) {
				return false;
			}
//			std::cout << "Unshot: " << total_unshot_r << "/" << total_unshot_g << "/" << total_unshot_b << std::endl;
			// compute form factors for patch (if necesary)
			Patch& selected_patch = patches_[selected_patch_index];
			if(!selected_patch.form_factors_computed_) {
				ComputeFormFactorsForPatch(cubes, patches_, selected_patch_index, [&](size_t i, size_t j, float q) {
					ff_.insert(i,j) = q;
					//ff_(i,j) = q;
				});
				selected_patch.form_factors_computed_ = true;
				//ff_computed_[selected_patch] = 1;
			}
			// shoot radiosity
			for(size_t j=0; j<quads_count_; j++) {
				Patch& pj = patches_[j];
				float ff = ff_.coeff(selected_patch_index, j);
				//float ff = ff_(selected_patch_index, j);
				float d_rad_r = pj.albedo_r_ * selected_patch.B_unshot_r_ * ff;
				float d_rad_g = pj.albedo_g_ * selected_patch.B_unshot_g_ * ff;
				float d_rad_b = pj.albedo_b_ * selected_patch.B_unshot_b_ * ff;
				pj.B_unshot_r_ += d_rad_r;
				pj.B_unshot_g_ += d_rad_g;
				pj.B_unshot_b_ += d_rad_b;
				pj.B_r_ += d_rad_r;
				pj.B_g_ += d_rad_g;
				pj.B_b_ += d_rad_b;
			}
			ambient_r_ = ambient_scl_r_ * (total_unshot_r - selected_patch.B_unshot_r_);
			ambient_g_ = ambient_scl_g_ * (total_unshot_g - selected_patch.B_unshot_g_);
			ambient_b_ = ambient_scl_b_ * (total_unshot_b - selected_patch.B_unshot_b_);
			selected_patch.B_unshot_r_ = 0.0f;
			selected_patch.B_unshot_g_ = 0.0f;
			selected_patch.B_unshot_b_ = 0.0f;
			return true;
		}

//		bool RadiosityShooting::ShootLightMT(Ptr(Cubes) cubes, unsigned int thread_count)
//		{
//			// prepare array for patch sorting and sum total undistributed light
//			std::vector<size_t> patch_ids(quads_count_);
//			float total_unshot_r = 0.0f;
//			float total_unshot_g = 0.0f;
//			float total_unshot_b = 0.0f;
//			for(size_t i=0; i<quads_count_; i++) {
//				patch_ids[i] = i;
//				total_unshot_r += B_unshot_r_(i);
//				total_unshot_g += B_unshot_g_(i);
//				total_unshot_b += B_unshot_b_(i);
//			}
//			// break if all light has been distributed
//			if(total_unshot_r + total_unshot_g + total_unshot_b == 0.0f) {
//				return false;
//			}
//			// find patches with maximum amount of undistributed light
//			std::partial_sort(patch_ids.begin(), patch_ids.begin() + thread_count, patch_ids.end(), [&](size_t a, size_t b) {
//				// use sum of channel light for comparison
//				float rad_a = PatchSelectionCriterion(B_unshot_r_(a), B_unshot_g_(a), B_unshot_b_(a));
//				float rad_b = PatchSelectionCriterion(B_unshot_r_(b), B_unshot_g_(b), B_unshot_b_(b));
//				return rad_a > rad_b;
//			});
//			patch_ids.resize(thread_count);
//			//std::cout << PatchSelectionCriterion(B_unshot_r_(patch_ids[0]), B_unshot_g_(patch_ids[0]), B_unshot_b_(patch_ids[0])) << std::endl;
//			// compute form factors for selected patches, one thread per patch
//			std::vector<boost::thread*> threads;
//			boost::mutex m;
//			for(size_t i=0; i<thread_count; i++) {
//				// create and run threads
//				threads.push_back(new boost::thread([&](size_t selected_patch) {
//					//std::cout << "Selected: " << selected_patch << std::endl;
//					if(!ff_computed_[selected_patch]) {
//						ComputeFormFactorsForPatch(cubes, cells_, selected_patch, [&](size_t i, size_t j, float q) {
//							boost::interprocess::scoped_lock<boost::mutex> l(m);
//							//std::cout << "(" << i << "," << j << ")=" << q << std::endl;
//							ff_.insert(i,j) = q;
//						});
//						ff_computed_[selected_patch] = 1;
//					}
//				}, patch_ids[i]));
//			}
//			// wait for form factor computation to complete
//			for(size_t i=0; i<threads.size(); i++) {
//				//std::cout << patch_ids[i] << " " << std::flush;
//				threads[i]->join();
//				delete threads[i];
//			}
//			// distribute light from selected patches to all other patches
//			for(size_t j=0; j<quads_count_; j++) {
//				float d_rad_r = 0.0f;
//				float d_rad_g = 0.0f;
//				float d_rad_b = 0.0f;
//				for(size_t i=0; i<thread_count; i++) {
//					size_t selected_patch = patch_ids[i];
//					float form_factor = ff_.coeff(selected_patch,j);
//					d_rad_r += B_unshot_r_(selected_patch) * form_factor;
//					d_rad_g += B_unshot_g_(selected_patch) * form_factor;
//					d_rad_b += B_unshot_b_(selected_patch) * form_factor;
//				}
//				d_rad_r *= albedo_r_(j);
//				d_rad_g *= albedo_g_(j);
//				d_rad_b *= albedo_b_(j);
//				B_unshot_r_(j) += d_rad_r;
//				B_unshot_g_(j) += d_rad_g;
//				B_unshot_b_(j) += d_rad_b;
//				B_r_(j) += d_rad_r;
//				B_g_(j) += d_rad_g;
//				B_b_(j) += d_rad_b;
//			}
//			// sum up distributed light and set value for undistributed light to 0 for processed patches
//			float shot_r = 0.0f;
//			float shot_g = 0.0f;
//			float shot_b = 0.0f;
//			for(size_t i=0; i<thread_count; i++) {
//				size_t selected_patch = patch_ids[i];
//				shot_r += B_unshot_r_(selected_patch);
//				shot_g += B_unshot_g_(selected_patch);
//				shot_b += B_unshot_b_(selected_patch);
//				B_unshot_r_(selected_patch) = 0.0f;
//				B_unshot_g_(selected_patch) = 0.0f;
//				B_unshot_b_(selected_patch) = 0.0f;
//			}
//			//std::cout << "Distributed " << shot_r << "/" << shot_g << "/" << shot_b << std::endl;
//			ambient_r_ = ambient_scl_r_ * (total_unshot_r - shot_r);
//			ambient_g_ = ambient_scl_g_ * (total_unshot_g - shot_g);
//			ambient_b_ = ambient_scl_b_ * (total_unshot_b - shot_b);
//			return true;
//		}

		void RadiosityShooting::Update(const Ptr(Cubes)& cubes)
		{
			size_t i = 0;
			float ambient = (ambient_r_ + ambient_g_ + ambient_b_) / 3.0f;
			for(auto it=cells_.cbegin(); it!=cells_.cend(); ++it) {
				for(Cell::BorderSideIterator cit=(*it)->IterateBorderSides(); cit; ++cit, i++) {
					Patch& p = patches_[i];
					CubeSideLightData light;
					light.ambient = 0.0f;
					light.sun = 0.0f;
					light.scenery = Vec3f(p.B_r_ + p.albedo_r_ * ambient, p.B_g_ + p.albedo_g_ * ambient, p.B_b_ + p.albedo_b_ * ambient);
					cit.data()->setCurrentLight(light, float(iteration_));
				}
				(*it)->SetAppearanceDirtyFlag();
			}
		}

	}
}
