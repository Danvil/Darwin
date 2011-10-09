#pragma once

#include "LinAlg.h"
#include "Coord.h"
#include <boost/random.hpp>

namespace Random
{
	namespace Private {
		struct BoostRandomEngineHolder {
			CANDY_API static boost::mt19937 rng;
		};
	}

	inline void Seed(unsigned int x)
	{
		Private::BoostRandomEngineHolder::rng.seed(x);
	}

	/** Random number in [a,b] */
	inline int Uniform(int a, int b)
	{
		boost::uniform_int<int> six(a, b);
		boost::variate_generator<boost::mt19937&, boost::uniform_int<int> > r(Private::BoostRandomEngineHolder::rng, six);
		return r();
	}

	/** Random number in [0,n] */
	inline int Uniform(int n)
	{
		return Uniform(0, n);
	}

	inline float Uniformf(float a, float b)
	{
		boost::uniform_real<float> six(a, b);
		boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > r(Private::BoostRandomEngineHolder::rng, six);
		return r();
	}

	/** Random number in [0,1] */
	inline float Uniformf()
	{
		return Uniformf(0.0f, 1.0f);
	}

	/** Random number in [-1,+1] */
	inline float UniformMPf()
	{
		return Uniformf(-1.0f, +1.0f);
	}

	inline float Gaussianf(float mu, float sigma)
	{
		boost::normal_distribution<float> six(mu, sigma);
		boost::variate_generator<boost::mt19937&, boost::normal_distribution<float> > r(Private::BoostRandomEngineHolder::rng, six);
		return r();
	}

	inline float Gaussianf()
	{
		return Gaussianf(0.0f, 1.0f);
	}

	inline float Gaussianf(float mu, float sigma, float max)
	{
		float n = Gaussianf(mu, sigma);
		return n < -max ? -max : (n > max ? max : n);
	}

	inline float GaussianHalff(float mu, float sigma)
	{
		float n = Gaussianf(0, sigma);
		return n < 0 ? mu-n : mu+n;
	}

	inline float GaussianHalff(float mu, float sigma, float max)
	{
		float n = Gaussianf(0, sigma);
		return std::min(n < 0 ? mu-n : mu+n, max);
	}

	inline Vec3f UniformVec3f(float ra, float rb, float rc) {
		return Vec3f(ra * Uniformf(), rb * Uniformf(), rc * Uniformf());
	}

	inline Vec3f UniformMPVec3f(float r) {
		return Vec3f(r * UniformMPf(), r * UniformMPf(), r * UniformMPf());
	}

	inline Vec3f UniformMPVec3f(float ra, float rb, float rc) {
		return Vec3f(ra * UniformMPf(), rb * UniformMPf(), rc * UniformMPf());
	}

	inline Vec3f UniformRandomInDisc(float r, float h=0.0f) {
		// FIXME improve speed!!!
		Vec3f x;
		do {
			x[0] = r * UniformMPf();
			x[1] = r * UniformMPf();
			x[2] = 0.0f;
		} while(x.squaredNorm() > r*r);
		x[2] = h;
		return x;
	}

	inline Vec3f UniformRandomInSphere(float r) {
		// FIXME improve speed!!!
		Vec3f x;
		do {
			x = UniformMPVec3f(r);
		} while(x.squaredNorm() > r*r);
		return x;
	}

	/** Uniform point distribution on the Z+ half sphere */
	void UniformOnHalfSphere(float& x, float& y, float& z);

	/** Uniform point distribution on the Z+ half sphere */
	inline Vec3f UniformOnHalfSphere() {
		float x, y, z;
		UniformOnHalfSphere(x, y, z);
		return Vec3f(x, y, z);
	}

	/** Pseudo uniform point distribution on the Z+ half sphere which selects between pre-computed samples */
	void PseudoUniformOnHalfSphere(float& x, float& y, float& z);

	inline Coord<int> UniformCoordI(int x1, int x2, int y1, int y2, int z1, int z2) {
		return Coord<int>(Uniform(x1, x2), Uniform(y1, y2), Uniform(z1, z2));
	}

}
