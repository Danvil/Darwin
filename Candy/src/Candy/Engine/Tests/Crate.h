#pragma once

#include "../../Candy_Dll.h"
#include "../IDrawable.h"
#include "../Shader.h"
#include "../Uniform.h"
#include "../Mesh.h"

namespace Candy
{
	/** A test cube with texture and normals for testing */
	class CANDY_API Crate
	: public IDrawable
	{
	public:
		Crate(float size);
		Crate(Ptr(PTN::Mesh) mesh);
		~Crate();
		void Render();

		void SetColor(const Vec4f& color) {
			u_color_->Change(color);
		}

	private:
		Ptr(PTN::Mesh) mesh_;
		Ptr(ShaderX) shader_;
		Ptr(Uniform4f) u_color_;
	};

}
