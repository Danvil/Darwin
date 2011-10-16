#pragma once

#include "../Candy_Dll.h"
#include <Candy/Tools/LinAlg.h>
#include <Candy/Cubes/Cube.h>
#include <string>

namespace Appearance
{
	CANDY_API void Load(const std::string& asset_dir);

	CubeMaterialProperties* GetCubeMaterialProperties(CubeType type);

	extern Vec3f AmbientColor;
	extern Vec3f SunColor;
	extern Vec3f SunPosition;

}
