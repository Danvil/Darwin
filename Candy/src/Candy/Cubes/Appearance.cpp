#include "Appearance.h"
#include <Candy/Tools/Image.h>
#include <cassert>

namespace Appearance
{
	void Load(const std::string& asset_dir)
	{
		std::string TextureName = asset_dir + "/cubes_diffuse_all.jpg";
		// load image
		Candy::Image3ub image = Candy::Load(TextureName);
		assert(image.width() == image.height() && image.width() == 2048); // FIXME must be power of two!
		const int cTiles = 4;
		assert(cTiles == 4); // FIXME must be power of two!
		unsigned int D = image.width() / cTiles;
		unsigned char* data = new unsigned char[3 * D * D];
		unsigned char* data_end = data + 3 * D * D;
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
		delete[] data;
		//
	}

	Vec3f BaseColorArray[16];

//	Vec3f AmbientColor = Vec3f(0.0f, 0.0f, 0.0f);
//	Vec3f SunColor = Vec3f(0.0f, 0.0f, 0.0f);

//	Vec3f AmbientColor = Vec3f(0.5f, 0.5f, 0.5f);
//	Vec3f SunColor = Vec3f(0.5f, 0.5f, 0.5f);

	Vec3f AmbientColor = 0.93f * Vec3f(67.0f/255.0f, 125.0f/255.0f, 212.0f/255.0f);
	Vec3f SunColor = 0.73f * Vec3f(0.90f, 0.92f, 0.88f);

	Vec3f SunPosition = Vec3f(0.7071f * 0.5f, 0.7071f * 0.5f, 0.866025f);

}
