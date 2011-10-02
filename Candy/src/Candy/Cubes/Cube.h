#pragma once

#include <Candy/Tools/LinAlg.h>

namespace CubeTypes
{
	enum Type
	{
		// "air"
		Empty = 0,

		// solid cubes
		Indestructible = 1,
		Stone = 10,
		Sand = 11,
		Ice = 12,
		Mud = 13,
//		Grass = 20,
		Water = 30,
		Lava = 31,
		CrystalGood = 41,
		CrystalEvil = 42,

		LightWhite = 50,
		LightRed = 51,
		LightGreen = 52,
		LightBlue = 53,

		DebugWhite = 130,
		DebugRed = 131,
		DebugGreen = 132,
		DebugBlue = 133,

		NonExistent = 255
	};
}

typedef CubeTypes::Type CubeType;

inline bool IsExisting(CubeType type) {
	return type != CubeTypes::NonExistent;
}

inline bool IsEmpty(CubeType type) {
	return type == CubeTypes::Empty;
}

inline bool IsEmptyOrNonExistent(CubeType type) {
	return type == CubeTypes::Empty || type == CubeTypes::NonExistent;
}

inline bool IsEmpty_NonExistentGround(CubeType type, bool above) {
	return type == CubeTypes::Empty || (type == CubeTypes::NonExistent && above);
}

inline bool IsSolid(CubeType type) {
	return type != CubeTypes::Empty && type != CubeTypes::NonExistent;
}

const int SunSliceCount = 13;

struct CubeSideLightData
{
	CubeSideLightData()
	: ambient(0), sun(0), scenery(0,0,0), scenery_with_object(0,0,0) {}
	
	CubeSideLightData(float amb, const Vec3f& scn)
	: ambient(amb), sun(0), scenery(scn) {}
	
	CubeSideLightData(float amb, const Vec3f& scn, const Vec3f& scn_wo)
	: ambient(amb), sun(0), scenery(scn), scenery_with_object(scn_wo) {}
	
	// ambient lighting value
	float ambient;

	// sun lighting value
	float sun;

	// scenery lighting value
	Vec3f scenery;

	Vec3f scenery_with_object;

};

struct CubeSideData
{
	CubeSideData() {} // FIXME no constructor!!!
	CubeSideLightData lighting; // one light value for each cube side
	Vec3f object_color; // all sides have the same base color
};

struct CubeInteriorData
{
	CubeType GetType() const {
		return (CubeType)type_8;
	}
	void SetType(CubeType type) {
		type_8 = (unsigned char)type;
	}
	bool IsType(CubeType type) const {
		return (CubeType)type_8 == type;
	}
	static CubeInteriorData CreateEmpty() {
		return CreateDefault(CubeTypes::Empty);
	}
	static CubeInteriorData CreateDefault(CubeType type) {
		CubeInteriorData x;
		x.SetType(type);
		return x;
	}
	float water;
	float water_vel_x;
	float water_vel_y;
	float water_vel_z;
private:
	unsigned char type_8;
};
