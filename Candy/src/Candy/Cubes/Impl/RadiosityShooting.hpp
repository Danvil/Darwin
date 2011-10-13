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

namespace Hexa
{
	namespace Lighting
	{
		/** Global illumination using an improved version of the radiosity algorithm */
		class RadiosityShooting
		: public GlobalIlluminator
		{
		public:
			RadiosityShooting();

			uint64_t Iterate(Ptr(Cubes) cubes);

		private:
			void Prepare(const Ptr(Cubes)& cubes);

			uint64_t ShootLight(Ptr(Cubes) cubes);
			bool ShootLightST(Ptr(Cubes) cubes);
			bool ShootLightMT(Ptr(Cubes) cubes, unsigned int thread_count);

			void Update(const Ptr(Cubes)& cubes);

		private:
		    std::vector<Cell*> cells_;
		    unsigned int quads_count_;
			Eigen::DynamicSparseMatrix<float,Eigen::RowMajor> ff_;
			std::vector<int> ff_computed_;
			Eigen::VectorXf albedo_r_, albedo_g_, albedo_b_;
			Eigen::VectorXf E_r_, E_g_, E_b_;
			Eigen::VectorXf B_r_, B_g_, B_b_;
			Eigen::VectorXf B_unshot_r_, B_unshot_g_, B_unshot_b_;
			float ambient_scl_r_, ambient_scl_g_, ambient_scl_b_;
			float ambient_r_, ambient_g_, ambient_b_;
		};
	}
}


#endif /* RADIOSITY_HPP_ */
