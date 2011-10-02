#include "Plt.h"
#include <Candy/Engine/Resources/ResourcePool.h>
#include <Candy/Engine/IDrawable.h>
#include <Candy/Engine/OpenGL.h>

using namespace std;

namespace CellRenderlings
{
	namespace PLT
	{

		size_t Layout::SizeInBytes() {
			return (3 + 2 + 1 + 1 + 3) * sizeof(float);
		}

		void Layout::SetupLayout() {
			unsigned int offset = 0;

			glVertexAttribPointer(0, 3, GL_FLOAT, false, GLsizeiptr(SizeInBytes()), BUFFER_OFFSET(offset));
			offset += 3 * sizeof(float);
			glVertexAttribPointer(1, 2, GL_FLOAT, false, GLsizeiptr(SizeInBytes()), BUFFER_OFFSET(offset));
			offset += 2 * sizeof(float);
			glVertexAttribPointer(2, 1, GL_FLOAT, false, GLsizeiptr(SizeInBytes()), BUFFER_OFFSET(offset));
			offset += 1 * sizeof(float);
			glVertexAttribPointer(3, 1, GL_FLOAT, false, GLsizeiptr(SizeInBytes()), BUFFER_OFFSET(offset));
			offset += 1 * sizeof(float);
			glVertexAttribPointer(4, 3, GL_FLOAT, false, GLsizeiptr(SizeInBytes()), BUFFER_OFFSET(offset));
			offset += 3 * sizeof(float);
			assert(offset == SizeInBytes());

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);
			glEnableVertexAttribArray(4);
		}

		Ptr(Candy::ShaderX) Renderling::shader_;
			
		Renderling::Renderling(Cubes* cubes, Cell* cell)
		: TriangleVerticeRenderling<Traits>(cubes, cell)
		{
		}


		Renderling::~Renderling()
		{
		}

		void Renderling::PrepareUpdateMesh()
		{
		}

		void Renderling::FinishUpdateMesh()
		{
		}

		void Renderling::PrepareRender()
		{
			if(shader_ == 0) {
				shader_.reset(new Candy::ShaderX());

				vector<string> attributes;
				attributes.push_back("in_position");
				attributes.push_back("in_uv");
				attributes.push_back("in_ambient");
				attributes.push_back("in_sun");
				attributes.push_back("in_scenery");
				vector<string> bindings;
				bindings.push_back("OutFragColor");
				Ptr(Candy::ShaderInfo) si = Candy::ResourcePool::Singleton->Load(
					"CubesPLT.vert",
					"CubesPLT.frag",
					attributes, bindings
				);
				shader_->SetProgram(Candy::ResourcePool::Singleton->Get(si));

				Candy::IDrawable::sCamera->AddCameraShaders(shader_);

				Ptr(Candy::TextureId) ti(new Candy::TextureId("cubes_diffuse_all.jpg"));
				shader_->AddTexture("uTexture", Candy::ResourcePool::Singleton->Get(ti));

				Candy::Uniform4f* ambient = new Candy::Uniform4f("uAmbientColor");
				ambient->Change(Appearance::AmbientColor, 1.0f);
				shader_->AddUniformStart(ambient);
				Candy::Uniform4f* sun = new Candy::Uniform4f("uSunColor");
				sun->Change(Appearance::SunColor, 1.0f);
				shader_->AddUniformStart(sun);
			}
			shader_->Apply();
		}

		void Renderling::FinishRender()
		{
		}

	}
}
