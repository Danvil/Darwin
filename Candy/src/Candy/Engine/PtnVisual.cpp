#include "PtnVisual.h"
#include "Resources.h"
using namespace std;

namespace Candy
{
	PtnVisual::PtnVisual(Ptr(PTN::Mesh) mesh, const std::string& frag, const std::string& vert, const std::string& tex_name)
	{
		mesh_ = mesh;
		shader_ = Candy::CreatePtnShader(frag, vert, tex_name);
	}

	PtnVisual::~PtnVisual()
	{
	}

	void PtnVisual::Render()
	{
		shader_->Apply();
		mesh_->Render();
	}
}
