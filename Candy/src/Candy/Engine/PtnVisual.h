#pragma once

#include "IDrawable.h"
#include "Shader.h"
#include "Uniform.h"
#include "Mesh.h"

namespace Candy
{
	/** A test cube with texture and normals for testing */
	class PtnVisual
	: public IDrawable
	{
	public:
		PtnVisual(Ptr(PTN::Mesh) mesh, const std::string& frag, const std::string& vert, const std::string& tex_name);
		~PtnVisual();
		void Render();

	private:
		Ptr(PTN::Mesh) mesh_;
		Ptr(ShaderX) shader_;
	};

}
