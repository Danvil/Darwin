#pragma once

#include "../Candy_Dll.h"
#include <Candy/Tools/LinAlg.h>
#include <Candy/Cubes/Cube.h>
#include <string>
#include <memory>

class CANDY_API Appearance
{
public:
	static std::shared_ptr<Appearance> singleton;

	static void Initialize(const std::string& asset_dir)
	{
		singleton.reset(new Appearance());
		singleton->Load(asset_dir);
	}

	void Load(const std::string& asset_dir);

	CubeMaterialProperties* GetCubeMaterialProperties(CubeType type)
	{ return cMaterials[static_cast<int>(type)]; }

	const Vec3f& AmbientColor() const
	{ return ambient_color_; }
	
	const Vec3f& SunColor() const
	{ return sun_color_; }
	
	const Vec3f& SunPosition() const
	{ return sun_position_; }
	
	Vec3f ambient_color_;
	Vec3f sun_color_;
	Vec3f sun_position_;

private:
	CubeMaterialProperties* cMaterials[256];
	Vec3f BaseColorArray[16];
};
