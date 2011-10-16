/*
 * Texture.cpp
 *
 *  Created on: 23.03.2009
 *      Author: david
 */

//---------------------------------------------------------------------------
#include "Texture.h"
#include "../OpenGL.h"
#include <IL/il.h>
#define ILUT_USE_OPENGL
#include <IL/ilut.h>
#include <stdexcept>
//---------------------------------------------------------------------------
namespace Candy {
//---------------------------------------------------------------------------

TextureSingle::TextureSingle(int type)
	: type_(type), id_(0)
{
}

TextureSingle::~TextureSingle()
{
}

TextureSingle::Texture TextureSingle::type() const
{
	switch(type_) {
	case GL_TEXTURE_1D:
		return Texture1D;
	case GL_TEXTURE_2D:
		return Texture2D;
	case GL_TEXTURE_3D:
		return Texture3D;
	case GL_TEXTURE_CUBE_MAP_EXT:
		return TextureCubeMap;
	default:
		throw std::string("Unknown texture type");
	}
}

void TextureSingle::Generate()
{
	glGenTextures(1, &id_);
}

void TextureSingle::Delete()
{
	glDeleteTextures(1, &id_);
}

void TextureSingle::Bind()
{
	AssureGenerated();
	glEnable(type_);
	glBindTexture(type_, id_);
}

void TextureSingle::DisableTexture()
{
	glDisable(type_);
	glBindTexture(type_, 0);
}

//---------------------------------------------------------------------------

bool Texture2::devil_initialized = false;

Texture2::Texture2()
:	TextureSingle(GL_TEXTURE_2D),
	width_(0), height_(0)
{
}

void Texture2::Load(const std::string& filename, bool linear, bool mipmaps)
{
	// bind texture
	Bind();
	// initialize DevIL
	if(!devil_initialized) {
		ilutRenderer(ILUT_OPENGL);
		devil_initialized = true;
	}
	// create image
	ILuint Id;
	ilGenImages(1, &Id);
	ilBindImage(Id);
	// load image from file
	bool success = ilLoadImage(filename.c_str()) == IL_TRUE;
	if(!success) {
		ilDeleteImages(1, &Id); // delete image
		throw std::runtime_error("Failed to load image from file: '" + filename + "'");
	}
	width_ = ilGetInteger(IL_IMAGE_WIDTH);
	height_ = ilGetInteger(IL_IMAGE_HEIGHT);
	// set opengl parameters
	Setup(linear, mipmaps);
	// load current image into current OpenGL texture
	success = ilutGLTexImage(0) == IL_TRUE;
	if(mipmaps) {
		// automagically
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	ilDeleteImages(1, &Id); // delete image
	if(!success) {
		throw std::string("Failed to load image into OpenGL");
	}
}

void Texture2::Load(const Image3ub& img, bool linear, bool mipmaps)
{
	Load(img.width(), img.height(), img.data(), linear, mipmaps);
}

void Texture2::Load(unsigned int width, unsigned int height, const unsigned char* data, bool linear, bool mipmaps)
{
	width_ = width;
	height_ = height;
	Bind();
	Setup(linear, mipmaps);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA8,
		(int)width, (int)height, 0,
		GL_RGB, GL_UNSIGNED_BYTE,
		data);
	if(mipmaps) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}
}

void Texture2::LoadTest(unsigned int width, unsigned int height, bool linear, bool mipmaps)
{
	width_ = width;
	height_ = height;
	unsigned char* data_begin = new unsigned char[3 * width * height];
	unsigned char* data = data_begin;
	for(unsigned int y=0; y < height; y++) {
		for(unsigned int x=0; x < width ; x++, data+=3) {
			data[0] = (unsigned char)(x % 255);
			data[1] = (unsigned char)(y % 255);
			data[2] = (unsigned char)((x*y) % 255);
		}
	}
	Load(width, height, data_begin, linear, mipmaps);
}

void Texture2::LoadChecker(unsigned int width, unsigned int height, bool linear, bool mipmaps)
{
	width_ = width;
	height_ = height;
	unsigned char* data_begin = new unsigned char[3 * width * height];
	unsigned char* data = data_begin;
	for(unsigned int y=0; y < height; y++) {
		for(unsigned int x=0; x < width ; x++, data+=3) {
			if(x < width/2 && y < height/2) {
				data[0] = 255; data[1] = 0; data[2] = 0;
			}
			else if(x < width/2 && y >= height/2) {
				data[0] = 0; data[1] = 255; data[2] = 0;
			}
			else if(x >= width/2 && y < height/2) {
				data[0] = 0; data[1] = 0; data[2] = 255;
			}
			else {
				data[0] = 255; data[1] = 255; data[2] = 255;
			}
		}
	}
	Load(width, height, data_begin, linear, mipmaps);
}

void Texture2::LoadWhite(unsigned int width, unsigned int height, bool linear, bool mipmaps)
{
	width_ = width;
	height_ = height;
	unsigned int n = 3 * width * height;
	unsigned char* data = new unsigned char[n];
	for(unsigned int p=0; p<n; p++) {
		data[p] = 255;
	}
	Load(width, height, data, linear, mipmaps);
}

void Texture2::Setup(bool linear, bool mipmaps)
{
	linear = false;
	if(mipmaps) {
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 8);
		if(linear) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		} else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR); // FIXME ???
		}
	} else {
		if(linear) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		} else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}
	}
}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
