#pragma once

#include <ostream>
#include <cassert>

template<typename K>
struct Coord
{
	K x, y, z;

	Coord() {}

	Coord(K px, K py, K pz)
		: x(px), y(py), z(pz) {}

	const K* data() const { return &x; }

	K* data() { return &x; }

	K operator[](unsigned int a) const {
		switch(a) {
		case 0: return x;
		case 1: return y;
		case 2: return z;
		default: assert(false);
		}
	}

	K& operator[](unsigned int a) {
		switch(a) {
		case 0: return x;
		case 1: return y;
		case 2: return z;
		default: assert(false);
		}
	}

	Coord<K>& operator+=(const Coord<K>& a) {
		x += a.x;
		y += a.y;
		z += a.z;
		return *this;
	}

	Coord<K>& operator-=(const Coord<K>& a) {
		x -= a.x;
		y -= a.y;
		z -= a.z;
		return *this;
	}

};

template<typename K>
Coord<K> operator*(K s, const Coord<K>& a) {
	return Coord<K>(s * a.x, s * a.y, s * a.z);
}

template<typename K>
Coord<K> operator+(const Coord<K>& a, const Coord<K>& b) {
	return Coord<K>(a.x + b.x, a.y + b.y, a.z + b.z);
}

template<typename K>
Coord<K> operator-(const Coord<K>& a, const Coord<K>& b) {
	return Coord<K>(a.x - b.x, a.y - b.y, a.z - b.z);
}

template<typename K>
bool operator==(const Coord<K>& a, const Coord<K>& b) {
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

template<typename K>
bool operator!=(const Coord<K>& a, const Coord<K>& b) {
	return a.x != b.x || a.y != b.y || a.z != b.z;
}

template<typename K>
std::ostream& operator<<(std::ostream& os, const Coord<K>& v) {
	os << "(" << v.x << "," << v.y << "," << v.z << ")";
	return os;
}

/*template<typename K>
K Distance(const Vec3<K>& a, const Vec3<K>& b) {
	K dx = a.x - b.x;
	K dy = a.y - b.y;
	K dz = a.z - b.z;
	return std::sqrt(dx * dx + dy * dy + dz * dz);
}

template<typename K>
K Length(const Vec3<K>& a) {
	return std::sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}*/

template<typename K>
bool operator<(const Coord<K>& a, const Coord<K>& b) {
	/*if(a.z == b.z) {
		if(a.y == b.y) {
			return a.x < b.x;
		} else {
			return a.y < b.y;
		}
	} else {
		return a.z < b.z;
	}*/
	return a.z < b.z || (a.z == b.z && (a.y < b.y || (a.y == b.y && a.x < b.x)));
}

typedef Coord<int> CoordI;
typedef Coord<unsigned int> CoordU;
typedef Coord<int> Ci;
typedef Coord<unsigned int> Cu;

struct CoordCompare
{
	static const unsigned int Bits = 5;
	static const unsigned int Mask = (1 << 5) - 1;

	static const unsigned int bucket_size = 1;

	static unsigned int I2U(int x) {
		return *((unsigned int*)(&x));
	}

	size_t operator()(const CoordI& a) const {
		size_t u = (I2U(a.x) & Mask) + ((I2U(a.y) & Mask) << Bits) + ((I2U(a.z) & Mask) << (2*Bits));
		return u;
	}

	/* returns true if a < b */
	bool operator()(const CoordI& a, const CoordI& b) const {
		return a < b; // use operator= defined above
	}
};
