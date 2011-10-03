#pragma once

#include <cmath>
#include <stdint.h>

namespace Math
{
	template<typename K>
	inline K Pi() {
		return K(3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679);
	}

	inline float PiF() {
		return Pi<float>();
	}

	template<typename K>
	inline K TwoPi() {
		return K(2) * Pi<K>();
	}

	inline float TwoPiF() {
		return TwoPi<float>();
	}

	template<typename K>
	inline K RadToDeg(K rad) {
		return rad / Pi<K>() * K(180);
	}

	template<typename K>
	inline K DegToRad(K deg) {
		return deg / K(180) * Pi<K>();
	}

	template<typename K>
	inline K Clamp(K x, K a, K b) {
		return (x < a ? a : (x > b ? b : x));
	}

	inline void ClampInplace(float& x, float a, float b) {
		x = (x < a ? a : (x > b ? b : x));
	}

	inline int ClampHalfOpen(int x, int a, int b) {
		if(x < a) {
			return a;
		}
		if(x >= b) {
			return b - 1;
		}
		return x;
	}

	inline bool IsInInterval(float x, float a, float b) {
		return a <= x && x <= b;
	}

	inline bool IsInHalfOpenInterval(int x, int a, int b) {
		return a <= x && x < b;
	}

	template<typename K>
	inline K Min(K a, K b, K c) {
		return std::min(a, std::min(b, c));
	}

	inline int Sign(float x) {
		return x < 0 ? -1 : +1;
	}

	inline float Square(float x) {
		return x * x;
	}

	union FastInverseSqrtUnion{
		float f;
		uint32_t i;
	};

	inline float FastInverseSqrt(float x) {
		FastInverseSqrtUnion a;
		a.f = x;
		//uint32_t i = *((uint32_t *)&x);			// evil floating point bit level hacking
		a.i = 0x5f3759df - (a.i >> 1);				// use a magic number
		float s = a.f;//*((float *)&i);				// get back guess
		return s * (1.5f - 0.5f * x * s * s);	// one newton iteration
	}

}
