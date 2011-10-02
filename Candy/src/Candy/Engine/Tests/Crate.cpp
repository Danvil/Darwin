#include "Crate.h"
#include "../Resources.h"
#include <GL/glew.h>
using namespace std;

namespace Candy
{
	Crate::Crate(float size)
	{
		mesh_ = MeshFactory::CreateBoxPTN(size);
		u_color_.reset(new Uniform4f("uColor", 1.0f, 1.0f, 1.0f, 1.0f));
	}

	Crate::Crate(Ptr(PTN::Mesh) mesh)
	{
		mesh_ = mesh;
	}

	Crate::~Crate()
	{
	}

	void Crate::Render()
	{
		if(!shader_) {
			shader_ = Candy::CreateCrateShader();
			shader_->AddUniformTick(u_color_.get());
		}
		shader_->Apply();
		mesh_->Render();
	}
}
