#pragma once

#include <ostream>

template<typename K>
struct Coord2
{
	K x, y;

	Coord2() {}

	Coord2(K px, K py)
		: x(px), y(py) {}

	const K* data() const { return &x; }

	K* data() { return &x; }

	Coord2<K>& operator+=(const Coord2<K>& a) {
		x += a.x;
		y += a.y;
		return *this;
	}

	Coord2<K>& operator-=(const Coord2<K>& a) {
		x -= a.x;
		y -= a.y;
		return *this;
	}

};

template<typename K>
Coord2<K> operator*(K s, const Coord2<K>& a) {
	return Coord2<K>(s * a.x, s * a.y);
}

template<typename K>
Coord2<K> operator+(const Coord2<K>& a, const Coord2<K>& b) {
	return Coord2<K>(a.x + b.x, a.y + b.y);
}

template<typename K>
Coord2<K> operator-(const Coord2<K>& a, const Coord2<K>& b) {
	return Coord2<K>(a.x - b.x, a.y - b.y);
}

template<typename K>
bool operator==(const Coord2<K>& a, const Coord2<K>& b) {
	return a.x == b.x && a.y == b.y;
}

template<typename K>
bool operator!=(const Coord2<K>& a, const Coord2<K>& b) {
	return a.x != b.x || a.y != b.y;
}

template<typename K>
std::ostream& operator<<(std::ostream& os, const Coord2<K>& v) {
	os << "(" << v.x << "," << v.y << ")";
	return os;
}

template<typename K>
bool operator<(const Coord2<K>& a, const Coord2<K>& b) {
	return a.y < b.y || (a.y == b.y && a.x < b.x);
}

typedef Coord2<int> Coord2I;
typedef Coord2<unsigned int> Coord2U;
typedef Coord2<int> C2i;
typedef Coord2<unsigned int> C2u;
