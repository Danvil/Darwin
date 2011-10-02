#include "Pnc.h"
#include "../../Appearance.h"
#include <Candy/Engine/Resources/ResourcePool.h>
#include <Candy/Engine/Shader.h>
#include <Candy/Engine/OpenGL.h>

using namespace std;

namespace CellRenderlings
{
	namespace PNC
	{

		size_t PncLayout::SizeInBytes() {
			return (3 + 3 + 3) * sizeof(float);
		}

		void PncLayout::SetupLayout() {
			int offset = 0;

			glVertexAttribPointer(0, 3, GL_FLOAT, false, GLsizeiptr(SizeInBytes()), BUFFER_OFFSET(offset));
			offset += 3 * sizeof(float);
			glVertexAttribPointer(1, 3, GL_FLOAT, false, GLsizeiptr(SizeInBytes()), BUFFER_OFFSET(offset));
			offset += 3 * sizeof(float);
			glVertexAttribPointer(2, 3, GL_FLOAT, false, GLsizeiptr(SizeInBytes()), BUFFER_OFFSET(offset));
			offset += 3 * sizeof(float);
			assert(offset == SizeInBytes());

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
		}

		PncRenderling::PncRenderling(Cubes* cubes, Cell* cell)
		: TriangleVerticeRenderling<PncTraits>(cubes, cell)
		{
		}

		Ptr(Candy::ShaderX) PncRenderling::shader_;

		PncRenderling::~PncRenderling()
		{
		}

		void PncRenderling::PrepareUpdateMesh()
		{
		}

		void PncRenderling::FinishUpdateMesh()
		{
		}

		void PncRenderling::PrepareRender()
		{
			if(shader_ == 0) {
				shader_.reset(new Candy::ShaderX());
				vector<string> attributes;
				attributes.push_back("in_position");
				attributes.push_back("in_normal");
				attributes.push_back("in_color");
				vector<string> bindings;
				bindings.push_back("OutFragColor");
				Ptr(Candy::ShaderInfo) info = Candy::ResourcePool::Singleton->Load(
					"CubesPNC.vert",
					"CubesPNC.frag",
					attributes, bindings
				);
				shader_->SetProgram(Candy::ResourcePool::Singleton->Get(info));
				Candy::IDrawable::sCamera->AddCameraShaders(shader_);
				Candy::Uniform4f* ambient = new Candy::Uniform4f("uAmbientColor");
				ambient->Change(Appearance::AmbientColor, 1.0f);
				shader_->AddUniformStart(ambient);
			}
			shader_->Apply();
		}

		void PncRenderling::FinishRender()
		{
		}

	}
}
