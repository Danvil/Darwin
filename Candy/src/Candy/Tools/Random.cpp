#include "Random.h"
#include <boost/random.hpp>

namespace Random
{
	namespace Private {
		boost::mt19937 BoostRandomEngineHolder::rng;
	}

	void UniformOnHalfSphere(float& x_out, float& y_out, float& z_out)
	{
		// use normal distributions on coordinate axes to get uniform distribution on sphere
		static boost::variate_generator<boost::mt19937&, boost::normal_distribution<float>> rnd(
				Private::BoostRandomEngineHolder::rng,
				boost::normal_distribution<float>());
		float x = rnd();
		float y = rnd();
		float z = rnd();
		float scl = 1.0f / std::sqrt(x*x + y*y + z*z);
		x_out = scl * x;
		y_out = scl * y;
		z_out = scl * z;

		// this is essentially the same as above, but much slower!
//		static boost::variate_generator<boost::mt19937&, boost::uniform_on_sphere<float> > rnd(
//				Private::BoostRandomEngineHolder::rng,
//				boost::random::uniform_on_sphere<float>(3));
//		std::vector<float> s = rnd();
//		x_out = s[0];
//		y_out = s[1];
//		z_out = s[2];

		// a primitive method for uniform points on a sphere
//		while(true) {
//			// find random point in half cube
//			float x = Random::UniformMPf(); // [-1,+1]
//			float y = Random::UniformMPf(); // [-1,+1]
//			float z = Random::Uniformf(); // [0,1]
//			// check if point is inside half sphere (and not zero)
//			float len2 = x * x + y * y + z * z;
//			if(0.0f <= len2 && len2 <= 1.0f) {
//				// normalize to sphere surface
//				float scl = 1.0f / std::sqrt(len2);
//				x_out = x * scl;
//				y_out = y * scl;
//				z_out = z * scl;
//				return;
//			}
//		}
	}

	namespace Impl
	{
		template<unsigned int N>
		struct PseudoHalfSphereChache
		{
			PseudoHalfSphereChache() {
				for(unsigned int i=0; i<N; i++) {
					Vec3f p = UniformOnHalfSphere();
					points_[i][0] = p.x();
					points_[i][1] = p.y();
					points_[i][2] = p.z();
				}
			}
			void Pick(unsigned int i, float& x, float& y, float& z) {
				x = points_[i][0];
				y = points_[i][1];
				z = points_[i][2];
			}
		private:
			float points_[N][3];
		};

		/**
		 * N: number of cached points
		 */
		template<unsigned int N>
		struct PseudoHalfSphereSampler_Random
		: private PseudoHalfSphereChache<N>
		{
			PseudoHalfSphereSampler_Random()
			: rnd_(Private::BoostRandomEngineHolder::rng, boost::uniform_int<unsigned int>(0, N-1))
			{}
			void Pick(float& x, float& y, float& z) {
				PseudoHalfSphereChache<N>::Pick(rnd_(), x, y, z);
			}
		private:
			boost::variate_generator<boost::mt19937&, boost::uniform_int<unsigned int> > rnd_;
		};

		/**
		 * N: number of cached points
		 * W: number of used points (a prime is probably good)
		 */
		template<unsigned int N, unsigned int W>
		struct PseudoHalfSphereSampler_Seq
		: private PseudoHalfSphereChache<N>
		{
			PseudoHalfSphereSampler_Seq()
			: next_(0)
			{}
			void Pick(float& x, float& y, float& z) {
				PseudoHalfSphereChache<N>::Pick(next_, x, y, z);
				next_ ++;
				if(next_ > W) {
					next_ = 0;
				}
			}
		private:
			unsigned int next_;
		};
	}

	void PseudoUniformOnHalfSphere(float& x, float& y, float& z) {
		//static Impl::PseudoHalfSphereSampler_Seq<64, 63> cache;
		static Impl::PseudoHalfSphereSampler_Random<128> cache;
		return cache.Pick(x, y, z);
	}

}
