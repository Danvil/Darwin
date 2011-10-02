#include "Shader.h"
#include "OpenGL.h"

namespace Candy
{

	ShaderX::ShaderX()
	{
	}

	ShaderX::~ShaderX()
	{
	}

	void ShaderX::SetProgram(Ptr(ShaderProgram) p)
	{
		program_ = p;
	}

	void ShaderX::AddTexture(const std::string& name, Ptr(TextureSingle) tex)
	{
		Uniform1i* u = new Uniform1i(name, textures_.size());
		// also add a uniform 
		textures_.push_back(tex);
		AddUniformStart(u);
	}

	void ShaderX::AddUniformStart(const Uniform* u)
	{
		uniforms_.AddToStart(u);
	}

	void ShaderX::AddUniformTick(const Uniform* u)
	{
		uniforms_.AddToTick(u);
	}

	void ShaderX::RemoveUniform(const Uniform* u)
	{
		uniforms_.Remove(u);
	}

	void ShaderX::ApplyStart()
	{
		program_->Use();
		for(size_t i=0; i<textures_.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			textures_[i]->Bind();
		}
		uniforms_.SetProgram(program_->getProgram()); // FIXME
		uniforms_.SetStart();
	}

	void ShaderX::ApplyTick()
	{
		uniforms_.SetTick();
	}

	void ShaderX::ApplyStop()
	{
	}

}
