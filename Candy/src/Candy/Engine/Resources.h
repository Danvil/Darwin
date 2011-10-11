#ifndef CANDY_RESOURCE_
#define CANDY_RESOURCE_

#include "Resources/ResourceInfo.h"
#include "Resources/Resource.h"
#include "Resources/ResourcePool.h"
#include "Resources/Texture.h"
#include "Resources/ShaderProgram.h"
#include "Resources/MeshFactory.h"
#include "Shader.h"
#include <Candy/Tools/Ptr.h>

namespace Candy
{
	Ptr(ShaderX) CreateCrateShader();

	Ptr(ShaderX) CreatePnShader(const std::string& vert, const std::string& frag);

	Ptr(ShaderX) CreatePtnShader(const std::string& vert, const std::string& frag, const std::string& tex_name, const std::string& tex_uniform_name="uTexture");

}

#endif
