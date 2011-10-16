#pragma once

#include <Candy/Tools/LinAlg.h>

namespace CubeTypes
{
	enum Type
	{
		// special types
		Empty = 0,
		NonExistent = 255,

		// solid cubes
		Concrete = 1,
		Stone = 2,
		Dirt = 3,
		Danvil = 4,
		AlaMinecraft = 5,

		// grey and white (diffuse)
		PlainGreyBegin = 114,
		PlainGrey0 = PlainGreyBegin,
		PlainGrey1 = PlainGreyBegin + 1,
		PlainGrey2 = PlainGreyBegin + 2,
		PlainGrey3 = PlainGreyBegin + 3,
		PlainGrey4 = PlainGreyBegin + 4,
		PlainGrey5 = PlainGreyBegin + 5,
		PlainGrey6 = PlainGreyBegin + 6,
		PlainGreyEnd = PlainGreyBegin + 7,
		PlainBlack = PlainGrey0,
		PlainWhite = PlainGrey6,

		// grey and white (emit)
		EmitGreyBegin = PlainGreyEnd,
		EmitGrey0 = EmitGreyBegin,
		EmitGrey1 = EmitGreyBegin + 1,
		EmitGrey2 = EmitGreyBegin + 2,
		EmitGrey3 = EmitGreyBegin + 3,
		EmitGrey4 = EmitGreyBegin + 4,
		EmitGrey5 = EmitGreyBegin + 5,
		EmitGrey6 = EmitGreyBegin + 6,
		EmitGreyEnd = EmitGreyBegin + 7,
		EmitBlack = EmitGrey0,
		EmitWhite = EmitGrey6,

		// strong colors (diffuse)
		PlainColorBegin	= EmitGreyEnd,
		PlainRed 		= PlainColorBegin,
		PlainOrange		= PlainRed+3,
		PlainYellow 	= PlainRed+6,
		PlainGreen 		= PlainRed+12,
		PlainCyan 		= PlainRed+18,
		PlainBlue 		= PlainRed+24,
		PlainMagenta 	= PlainRed+30,
		PlainColorEnd	= PlainRed+36,

		// strong colors (emit)
		EmitColorBegin	= PlainColorEnd,
		EmitRed			= EmitColorBegin,
		EmitOrange		= EmitRed+3,
		EmitYellow 		= EmitRed+6,
		EmitGreen 		= EmitRed+12,
		EmitCyan 		= EmitRed+18,
		EmitBlue		= EmitRed+24,
		EmitMagenta		= EmitRed+30,
		EmitColorEnd	= EmitRed+36,

		// deprecated
		Indestructible = Stone,
		Sand = Dirt,
		Mud = Dirt,
		Water = PlainCyan,
		Lava = EmitYellow,
		LightWhite = EmitWhite,
		LightRed = EmitRed,
		LightGreen = EmitGreen,
		LightBlue = EmitBlue,
		LightYellow = EmitYellow,
		LightCyan = EmitCyan,
		LightMagenta = EmitMagenta,
		DebugWhite = PlainWhite,
		DebugRed = PlainRed,
		DebugGreen = PlainGreen,
		DebugBlue = PlainBlue,


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

struct CubeMaterialProperties
{
	CubeMaterialProperties()
	: texture_index_(0), color_(1.0f,1.0f,1.0f), albedo_(0.7f,0.7f,0.7f), emit_(0.0f,0.0f,0.0f) {}

	CubeMaterialProperties(size_t texture_index, const Vec3f& color)
	: texture_index_(texture_index), color_(color), albedo_(0.7f * color_), emit_(0.0f,0.0f,0.0f) {}

	CubeMaterialProperties(size_t texture_index, const Vec3f& color, const Vec3f& emit)
	: texture_index_(texture_index), color_(color), albedo_(0.7f * color_), emit_(emit) {}

	/** Texture index */
	size_t texture_index_;

	/** Diffuse color */
	Vec3f color_;

	/** Surface albedo */
	Vec3f albedo_;

	/** Lighting emit color (and strength) */
	Vec3f emit_;
};

struct CubeSideData
{
	CubeSideData()
	: weight_old_(0.0f), weight_new_(1.0f) {}

	void setMaterial(const CubeMaterialProperties* m) {
		material_ = m;
	}

	const CubeMaterialProperties* getMaterial() const {
		return material_;
	}

	void set(const CubeSideLightData& total_data, float weight) {
		lighting_old_ = total_data;
		weight_old_ = weight;
		lighting_new_ = total_data;
		weight_new_ = weight;
		lighting_current_ = total_data;
		weight_current_ = weight;
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

private:
	void update() {
		float q = std::min(1.0f, weight_new_ / std::min(weight_old_, float(1024)));
		float p = 1.0f - q;
		weight_current_ = p * weight_old_ + q * weight_new_;
		lighting_current_.ambient	= p * lighting_old_.ambient	+ q * lighting_new_.ambient;
		lighting_current_.sun		= p * lighting_old_.sun		+ q * lighting_new_.sun;
		lighting_current_.scenery	= p * lighting_old_.scenery	+ q * lighting_new_.scenery;
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

	/** Material properties */
	const CubeMaterialProperties* material_;
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
