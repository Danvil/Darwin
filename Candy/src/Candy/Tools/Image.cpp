#include "Image.h"
#include <IL/il.h>
#include <cassert>
#include <stdexcept>
#include <iostream>

namespace Candy
{

	Image3ub Load(const std::string& filename)
	{
		// load image
		ILuint tex_image = ilGenImage();
		ilBindImage(tex_image);
		ILboolean success = ilLoadImage(filename.c_str());
		if(!success) {
			std::cerr << "DevIL error: " << ilGetError() << std::endl;
			throw std::runtime_error("Error while loading image!");
		}
		// get dimensions
		ILuint width, height;
		width = ilGetInteger(IL_IMAGE_WIDTH);
		height = ilGetInteger(IL_IMAGE_HEIGHT);
		Image3ub img(width, height);
		// get pixel data
		ilCopyPixels(0, 0, 0, width, height, 1, IL_RGB, IL_UNSIGNED_BYTE, img.data().get());
		// free image
		ilDeleteImage(tex_image);
		// ready
		return img;
	}

}
