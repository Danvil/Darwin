#pragma once

#include "../Candy_Dll.h"
#include <Candy/Tools/LinAlg.h>
#include <Candy/Cubes/Cube.h>
#include <string>

namespace Appearance
{
	CANDY_API void Load(const std::string& asset_dir);

	inline unsigned int CubeTypeToTextureIndex(CubeType type) {
		switch(type) {
//		case CubeTypes::Grass: return 0;
		case CubeTypes::Mud: return 1;
		case CubeTypes::Stone: return 2;
		case CubeTypes::Ice: return 4;
		case CubeTypes::Indestructible: return 5;
		case CubeTypes::Lava: return 6;
		case CubeTypes::Sand: return 7;
		case CubeTypes::Water: return 8;
		case CubeTypes::CrystalGood: return 10;
		case CubeTypes::CrystalEvil: return 9;
		case CubeTypes::LightWhite: return 15;
		case CubeTypes::LightRed: return 12;
		case CubeTypes::LightGreen: return 13;
		case CubeTypes::LightBlue: return 14;
		case CubeTypes::LightYellow: return 9;
		case CubeTypes::LightCyan: return 10;
		case CubeTypes::LightMagenta: return 11;
		case CubeTypes::DebugWhite: return 15;
		case CubeTypes::DebugRed: return 12;
		case CubeTypes::DebugGreen: return 13;
		case CubeTypes::DebugBlue: return 14;
		default: return 15;
		}
	}

	extern Vec3f BaseColorArray[16];

	inline const Vec3f& CubeBaseColor(CubeType type) {
		return BaseColorArray[CubeTypeToTextureIndex(type)];
	}

	inline int CubeTypeToEmitColorIndex(CubeType type) {
		switch(type) {
		case CubeTypes::LightWhite: return 1;
		case CubeTypes::LightRed: return 2;
		case CubeTypes::LightGreen: return 3;
		case CubeTypes::LightBlue: return 4;
		case CubeTypes::LightYellow: return 5;
		case CubeTypes::LightCyan: return 6;
		case CubeTypes::LightMagenta: return 7;
		case CubeTypes::Lava: return 8;
		case CubeTypes::Water: return 9;
		case CubeTypes::CrystalGood: return 10;
		case CubeTypes::CrystalEvil: return 11;
		default: return 0;
		}
	}

	const Vec3f EmitColorArray[] = {
		Vec3f(0,0,0), // none
		Vec3f(5,5,5), // light white
		Vec3f(5,2,2), // light red
		Vec3f(2,5,2), // light green
		Vec3f(2,2,5), // light blue
		Vec3f(4.0,3.5,1.5), // light yellow
		Vec3f(1.0,4.5,4.5), // light cyan
		Vec3f(4.0,1.0,4.0), // light magenta
		Vec3f(1.5f,1.35f,1.20f), // lava
		Vec3f(0.05f,0.07f,0.10f), // Water
		Vec3f(0.75f,0.80f,0.00f), // CrystalGood
		Vec3f(0.85f,0.00f,0.75f) // CrystalEvil
	};

	inline const Vec3f& CubeEmitColor(CubeType type) {
		return EmitColorArray[CubeTypeToEmitColorIndex(type)];
	}

	extern Vec3f AmbientColor;
	extern Vec3f SunColor;
	extern Vec3f SunPosition;

}
