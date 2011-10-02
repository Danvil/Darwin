#include "Resources.h"
#include "Resources/ResourcePool.h"
#include "IDrawable.h"
#include "Shader.h"
#include <vector>
#include <string>
using namespace std;

namespace Candy
{
	Ptr(ShaderX) CreateCrateShader()
	{
		return CreatePtnShader("SimplePTN.vert", "Tex.frag", "testtex.jpg");
	}

	Ptr(ShaderX) CreatePnShader(const std::string& vert, const std::string& frag)
	{
		Ptr(ShaderX) shader(new ShaderX());

		vector<string> attributes;
		attributes.push_back("in_position");
		attributes.push_back("in_normal");
		vector<string> bindings;
		bindings.push_back("OutFragColor");
		Ptr(ShaderInfo) si = ResourcePool::Singleton->Load(
			vert, frag,
			attributes, bindings
		);
		shader->SetProgram(ResourcePool::Singleton->Get(si));

		IDrawable::sCamera->AddCameraShaders(shader);

		return shader;
	}

	Ptr(ShaderX) CreatePtnShader(const std::string& vert, const std::string& frag, const std::string& tex_name)
	{
		Ptr(ShaderX) shader(new ShaderX());

		vector<string> attributes;
		attributes.push_back("in_position");
		attributes.push_back("in_normal");
		attributes.push_back("in_uv");
		vector<string> bindings;
		bindings.push_back("OutFragColor");
		Ptr(ShaderInfo) si = ResourcePool::Singleton->Load(
			vert, frag,
			attributes, bindings
		);
		shader->SetProgram(ResourcePool::Singleton->Get(si));

		IDrawable::sCamera->AddCameraShaders(shader);

		Ptr(TextureId) ti(new TextureId(tex_name));
		shader->AddTexture("uTexture", ResourcePool::Singleton->Get(ti));

		return shader;
	}

}
