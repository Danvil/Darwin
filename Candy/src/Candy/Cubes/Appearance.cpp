#include "Appearance.h"
#include <Candy/Tools/Image.h>
#include <cassert>
#include <map>

namespace Appearance
{
	CubeMaterialProperties* cMaterials[256];

	Vec3f BaseColorArray[16];

	void HSV_2_RGB(float h, float s, float v, float& r, float& g, float& b) {
		int i = (unsigned int)(std::floor(h * 6.0f));
		float f = h * 6.0f - float(i);
		float p = v * (1.0f - s);
		float q = v * (1.0f - s * f);
		float t = v * (1.0f - s * (1.0f - f));
		switch(i) {
		case 0: r = v; g = t; b = p; break;
		case 1: r = q; g = v; b = p; break;
		case 2: r = p; g = v; b = t; break;
		case 3: r = p; g = q; b = v; break;
		case 4: r = t; g = p; b = v; break;
		case 5: r = v; g = p; b = q; break;
		default: r = 0; g = 0; b = 0; break;
		}
	}

	void Load(const std::string& asset_dir)
	{
		// load texture
		std::string TextureName = asset_dir + "/Texture_Composite.png";
		// load image
		Candy::Image3ub image = Candy::Load(TextureName);
		assert(image.width() == image.height() && image.width() == 2048); // FIXME must be power of two!
		const int cTiles = 4;
		assert(cTiles == 4); // FIXME must be power of two!
		unsigned int D = image.width() / cTiles;
//		unsigned char* data = new unsigned char[3 * D * D];
//		unsigned char* data_end = data + 3 * D * D;
		// get base colors
		for(unsigned int i=0; i<16; i++) {
			unsigned int x_base = (i & (cTiles-1)) * D;
			unsigned int y_base = (i / cTiles) * D;
			// compute mean color
			float mean_r=0, mean_g=0, mean_b=0;
			for(unsigned int y=y_base; y<y_base+D; y++) {
				for(unsigned int x=x_base; x<x_base+D; x++) {
					unsigned char* p = image.data().get() + 3*(x + y*image.width());
					mean_r += float(*(p++));
					mean_g += float(*(p++));
					mean_b += float(*(p++));
				}
			}
			mean_r *= 1.0f / float(D * D) / 255.0f;
			mean_g *= 1.0f / float(D * D) / 255.0f;
			mean_b *= 1.0f / float(D * D) / 255.0f;
			BaseColorArray[i] = Vec3f(mean_r, mean_g, mean_b);
		}
//		delete[] data;
		// create materials
		cMaterials[CubeTypes::Concrete]		= new CubeMaterialProperties(1, BaseColorArray[1]);
		cMaterials[CubeTypes::Stone]		= new CubeMaterialProperties(2, BaseColorArray[2]);
		cMaterials[CubeTypes::Dirt]			= new CubeMaterialProperties(3, BaseColorArray[3]);
		cMaterials[CubeTypes::Danvil]		= new CubeMaterialProperties(4, BaseColorArray[4]);
		cMaterials[CubeTypes::AlaMinecraft]	= new CubeMaterialProperties(5, BaseColorArray[5]);
		for(int i=int(CubeTypes::PlainGreyBegin); i<int(CubeTypes::PlainGreyEnd); i++) {
			float I = float(i - int(CubeTypes::PlainGreyBegin)) / float(int(CubeTypes::PlainGreyEnd) - int(CubeTypes::PlainGreyBegin) - 1);
			cMaterials[i] = new CubeMaterialProperties(0, Vec3f(I,I,I));
		}
		for(int i=int(CubeTypes::EmitGreyBegin); i<int(CubeTypes::EmitGreyEnd); i++) {
			float I = float(i - int(CubeTypes::EmitGreyBegin)) / float(int(CubeTypes::EmitGreyEnd) - int(CubeTypes::EmitGreyBegin) - 1);
			cMaterials[i] = new CubeMaterialProperties(0, Vec3f(I,I,I), 5.0f*Vec3f(I,I,I));
		}
		for(int i=int(CubeTypes::PlainColorBegin); i<int(CubeTypes::PlainColorEnd); i++) {
			float hue = float(i - int(CubeTypes::PlainColorBegin)) / float(int(CubeTypes::PlainColorEnd) - int(CubeTypes::PlainColorBegin));
			float r, g, b;
			HSV_2_RGB(hue, 1.0f, 1.0f, r, g, b);
			cMaterials[i] = new CubeMaterialProperties(0, Vec3f(r,g,b));
		}
		for(int i=int(CubeTypes::EmitColorBegin); i<int(CubeTypes::EmitColorEnd); i++) {
			float hue = float(i - int(CubeTypes::EmitColorBegin)) / float(int(CubeTypes::EmitColorEnd) - int(CubeTypes::EmitColorBegin));
			float r, g, b;
			HSV_2_RGB(hue, 1.0f, 1.0f, r, g, b);
			cMaterials[i] = new CubeMaterialProperties(0, Vec3f(r,g,b), 5.0f*Vec3f(r,g,b));
		}
		for(size_t i=0; i<256; i++) {
			if(cMaterials[i] == 0) {
				cMaterials[i] = cMaterials[int(CubeTypes::PlainWhite)];
			}
		}
	}

	CubeMaterialProperties* GetCubeMaterialProperties(CubeType type) {
		return cMaterials[int(type)];
	}

//	Vec3f AmbientColor = Vec3f(0.0f, 0.0f, 0.0f);
//	Vec3f SunColor = Vec3f(0.0f, 0.0f, 0.0f);

//	Vec3f AmbientColor = Vec3f(0.5f, 0.5f, 0.5f);
//	Vec3f SunColor = Vec3f(0.5f, 0.5f, 0.5f);

	Vec3f AmbientColor = 0.93f * Vec3f(67.0f/255.0f, 125.0f/255.0f, 212.0f/255.0f);
	Vec3f SunColor = 0.73f * Vec3f(0.90f, 0.92f, 0.88f);

	Vec3f SunPosition = Vec3f(0.7071f * 0.5f, 0.7071f * 0.5f, 0.866025f);

}
