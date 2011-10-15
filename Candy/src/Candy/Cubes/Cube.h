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
		LightYellow = 54,
		LightCyan = 55,
		LightMagenta = 56,

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
	: ambient(0), sun(0), scenery(0,0,0) {}
	
	CubeSideLightData(const Vec3f& scn)
	: ambient(0), sun(0), scenery(scn) {}

	CubeSideLightData(float amb, const Vec3f& scn)
	: ambient(amb), sun(0), scenery(scn) {}
	
	// ambient lighting value
	float ambient;

	// sun lighting value
	float sun;

	// scenery lighting value
	Vec3f scenery;

};

struct CubeSideData
{
	CubeSideData()
	: weight_old_(0.0f), weight_new_(1.0f) {}

	void set(const CubeSideLightData& total_data, float weight) {
		lighting_old_ = total_data;
		weight_old_ = weight;
		lighting_new_ = total_data;
		weight_new_ = weight;
		lighting_current_ = total_data;
		weight_current_ = weight;
		scenery_with_object_ = lighting_current_.scenery.cwiseProduct(object_color_);
	}

	void mark() {
		lighting_old_ = lighting_current_;
		weight_old_ = weight_current_;
		lighting_new_ = lighting_current_;
		weight_new_ = 0.0f;
	}

	void setCurrentLight(const CubeSideLightData& total_data, float total_weight) {
		lighting_new_ = total_data;
		weight_new_ = total_weight;
		update();
	}

	/**
	 * @param delta_data_times_delta_weight delta lighting values multiplied with delta weight
	 * @param delta_weight delta weight
	 */
	void addCurrentLight(const CubeSideLightData& delta_data_times_delta_weight, float delta_weight) {
		float s = 1.0f / float(weight_new_ + delta_weight);
		float s1 = weight_new_ * s;
		float s2 = s; // delta_data_times_delta_weight is already multiplied with the weight
		lighting_new_.ambient = s1 * lighting_new_.ambient + s2 * delta_data_times_delta_weight.ambient;
		lighting_new_.sun = s1 * lighting_new_.sun + s2 * delta_data_times_delta_weight.sun;
		lighting_new_.scenery = s1 * lighting_new_.scenery + s2 * delta_data_times_delta_weight.scenery;
		weight_new_ += delta_weight;
		update();
	}

	const CubeSideLightData& getLighting() const {
		return lighting_current_;
	}

	float getAmbient() const {
		return lighting_current_.ambient;
	}

	float getSun() const {
		return lighting_current_.sun;
	}

	const Vec3f& getScenery() const {
		return lighting_current_.scenery;
	}

	const Vec3f& getSceneryWithObject() const {
		return scenery_with_object_;
	}

	void setObjectColor(const Vec3f& object_color) {
		object_color_ = object_color;
	}

	const Vec3f& getObjectColor() const {
		return object_color_;
	}

private:
	void update() {
		float q = std::min(1.0f, weight_new_ / std::min(weight_old_, float(1024)));
		float p = 1.0f - q;
		weight_current_ = p * weight_old_ + q * weight_new_;
		lighting_current_.ambient	= p * lighting_old_.ambient	+ q * lighting_new_.ambient;
		lighting_current_.sun		= p * lighting_old_.sun		+ q * lighting_new_.sun;
		lighting_current_.scenery	= p * lighting_old_.scenery	+ q * lighting_new_.scenery;
		scenery_with_object_ = lighting_current_.scenery.cwiseProduct(object_color_);
	}

private:
	/** Last light */
	CubeSideLightData lighting_old_;

	/** Weight of old lighting */
	float weight_old_;

	/** Last light */
	CubeSideLightData lighting_new_;

	/** Weight of new lighting */
	float weight_new_;

	CubeSideLightData lighting_current_;

	float weight_current_;

	Vec3f scenery_with_object_;

	/** Cube side base color */
	Vec3f object_color_; // all sides have the same base color
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
