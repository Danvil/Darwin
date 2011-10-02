#ifndef CANDY_SHADER_
#define CANDY_SHADER_

#include "Uniform.h"
#include "Resources/ShaderProgram.h"
#include "Resources/Texture.h"
#include <Candy/Tools/Ptr.h>
#include <vector>

namespace Candy
{

	class ShaderX
	{
	public:
		ShaderX();
		~ShaderX();

		void SetProgram(Ptr(ShaderProgram) p);

		void AddTexture(const std::string& name, Ptr(TextureSingle) tex);

		/** Adds a uniform variable to the shader. */
		void AddUniformStart(const Uniform* u);

		void AddUniformTick(const Uniform* u);

		/** Removes the uniform variable. */
		void RemoveUniform(const Uniform* u);

		void ApplyStart();
		void ApplyTick();
		void ApplyStop();

		void Apply() {
			ApplyStart();
			ApplyTick();
			ApplyStop();
		}

	private:
		Ptr(ShaderProgram) program_;
		ShaderUniforms uniforms_;
		std::vector<Ptr(TextureSingle)> textures_;

	};

}

#endif
