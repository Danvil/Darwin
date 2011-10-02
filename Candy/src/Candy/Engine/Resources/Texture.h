/*
 * Texture2.h
 *
 *  Created on: 23.03.2009
 *      Author: david
 */

#ifndef DANVIL_SIMPLEENGINE_TEXTURE2_H_
#define DANVIL_SIMPLEENGINE_TEXTURE2_H_
//---------------------------------------------------------------------------
#include "Resource.h"
#include <Candy/Tools/Image3ub.h>
#include <string>
//---------------------------------------------------------------------------
namespace Candy {
//---------------------------------------------------------------------------

class ITexture
: public Ressource
{
public:
	virtual ~ITexture() {}
};

//---------------------------------------------------------------------------

class TextureSingle
: public ITexture
{
public:
	enum Texture {
		Texture1D, Texture2D, Texture3D, TextureCubeMap
	};

public:
	TextureSingle(int type);

	virtual ~TextureSingle();

	unsigned int Id() const { return id_; }

	void Bind();

	void Generate();

	void Delete();

	void DisableTexture();

protected:
	Texture type() const;

private:
	int type_;
	unsigned int id_;

};

//---------------------------------------------------------------------------

class Texture2
: public TextureSingle
{
public:
	Texture2();

	void Load(const std::string& filename, bool linear=true, bool mipmaps=true);

	void Load(const Image3ub& img, bool linear=true, bool mipmaps=true);

	void Load(unsigned int width, unsigned int height, const unsigned char* data, bool linear=true, bool mipmaps=true);

	void LoadTest(unsigned int width, unsigned int height, bool linear=true, bool mipmaps=true);
	
	void LoadChecker(unsigned int width, unsigned int height, bool linear=true, bool mipmaps=true);
	
	void LoadWhite(unsigned int width, unsigned int height, bool linear=true, bool mipmaps=false);

	unsigned int width() const {
		return width_;
	}

	unsigned int height() const {
		return height_;
	}

private:
	void Setup(bool linear, bool mipmaps);

private:
	unsigned int width_, height_;

private:
	static bool devil_initialized;

};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
