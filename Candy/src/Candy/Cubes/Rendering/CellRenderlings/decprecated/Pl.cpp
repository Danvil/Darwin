#include "Pl.h"
#include <Candy/Shader.h>
#include <Candy/OpenGL.h>
#include <Core.h>

using namespace std;

namespace CellRenderlings
{
	namespace PL
	{

		size_t PlLayout::SizeInBytes() {
			return (3 + 1 + 3) * sizeof(float);
		}

		void PlLayout::SetupLayout() {
			int offset = 0;

			glVertexAttribPointer(0, 3, GL_FLOAT, false, GLsizeiptr(SizeInBytes()), BUFFER_OFFSET(offset));
			offset += 3 * sizeof(float);
			glVertexAttribPointer(1, 1, GL_FLOAT, false, GLsizeiptr(SizeInBytes()), BUFFER_OFFSET(offset));
			offset += 1 * sizeof(float);
			glVertexAttribPointer(2, 3, GL_FLOAT, false, GLsizeiptr(SizeInBytes()), BUFFER_OFFSET(offset));
			offset += 3 * sizeof(float);
			assert(offset == SizeInBytes());

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
		}

		PlRenderling::PlRenderling(Cubes* cubes, Cell* cell)
		: TriangleVerticeRenderling<PlTraits>(cubes, cell)
		{
			vector<string> attributes;
			attributes.push_back("in_position");
			attributes.push_back("in_ambient");
			attributes.push_back("in_scenery");
			vector<string> bindings;
			bindings.push_back("OutFragColor");
			Candy::ShaderInfo info = Candy::ShaderInfo::Load(
				"Assets/Shaders/Cubes/PL.vert",
				"Assets/Shaders/Cubes/PL.frag",
				attributes, bindings
			);
			shader_ = new Candy::ShaderProgram(info);
			shader_->addUniform(Core::UniformProj);
			shader_->addUniform(Core::UniformView);
			shader_->addUniform(Core::UniformAmbientColor);
		}


		PlRenderling::~PlRenderling()
		{
		}

		void PlRenderling::PrepareUpdateMesh()
		{
		}

		void PlRenderling::FinishUpdateMesh()
		{
		}

		void PlRenderling::PrepareRender()
		{
			glDisable(GL_CULL_FACE);
			shader_->Use();
		}

		void PlRenderling::FinishRender()
		{
		}

	}
}
