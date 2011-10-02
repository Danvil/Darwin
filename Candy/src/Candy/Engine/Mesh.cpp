#include "Mesh.h"
#include "OpenGL.h"

namespace Candy 
{

	Mesh::Mesh(IMeshVertexLayout* layout) {
		_layout = layout;
		MustUpdateVertexData = false;
		MustUpdateIndexData = false;
		has_called_setup_layout = false;
		_indices_count = 0;
		vaoHandle = -1;
		_type = MeshTypeTriangles;
	}

	void Mesh::UpdateIndices(uint32_t* indices, size_t count) {
		_indices = indices;
		_indices_count = count;
		MustUpdateIndexData = true;
	}

	void Mesh::UpdateVertices(void* vertices, size_t count) {
		_vertices = vertices;
		_vertices_count = count;
		MustUpdateVertexData = true;
	}

	void Mesh::RenderStart() {
		if(_indices_count == 0 && !MustUpdateIndexData) {
			return;
		}
		AssureGenerated();
		Bind();
		if(MustUpdateIndexData) {
			UpdateIndexData();
		}
		if(MustUpdateVertexData) {
			UpdateVertexData();
		}
		if(!has_called_setup_layout) {
			_layout->SetupLayout();
			has_called_setup_layout = true;
		}
	}

	void Mesh::RenderTick() {
		switch(_type) {
		case MeshTypePoints:
			glDrawElements(GL_POINTS, _indices_count, GL_UNSIGNED_INT, 0);
			break;
		case MeshTypeLines:
			glDrawElements(GL_LINES, _indices_count, GL_UNSIGNED_INT, 0);
			break;
		case MeshTypeTriangles:
			glDrawElements(GL_TRIANGLES, _indices_count, GL_UNSIGNED_INT, 0);
			break;
		}
	}

	void Mesh::RenderStop() {
	}

	void Mesh::UpdateVertexData() {
		// transfer vertex data
		glBindBuffer(GL_ARRAY_BUFFER, vboHandle0);
		glBufferData(GL_ARRAY_BUFFER, _vertices_count * _layout->SizeInBytes(), static_cast<GLvoid*>(_vertices), GL_STATIC_DRAW);
		MustUpdateVertexData = false;
	}

	void Mesh::UpdateIndexData() {
		// transfer index data
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandle1);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices_count * sizeof(uint32_t), static_cast<GLvoid*>(_indices), GL_STATIC_DRAW);
		MustUpdateIndexData = false;
	}

	void Mesh::Bind() {
		// bind buffers 
		glBindVertexArray(vaoHandle);
		glBindBuffer(GL_ARRAY_BUFFER, vboHandle0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandle1);
	}

	void Mesh::Generate() {
		// gen vertex array object
		glGenVertexArrays(1, &vaoHandle);
		// gen vertex buffer object
		glBindVertexArray(vaoHandle);

		glGenBuffers(1, &vboHandle0);
		glGenBuffers(1, &vboHandle1);
	}

	void Mesh::Delete() {
		glDeleteBuffers(1, &vboHandle0);
		glDeleteBuffers(1, &vboHandle1);
		glDeleteVertexArrays(1, &vaoHandle);
	}

	namespace P
	{
		size_t Layout::SizeInBytes() {
			return 3 * sizeof(float);
		}

		void Layout::SetupLayout() {
			glVertexAttribPointer(0, 3, GL_FLOAT, false, GLsizeiptr(SizeInBytes()), BUFFER_OFFSET(0));
			glEnableVertexAttribArray(0);
		}
	}

	namespace PT
	{
		size_t Layout::SizeInBytes() {
			return (3 + 3) * sizeof(float);
		}

		void Layout::SetupLayout() {
			unsigned int offset = 0;
			glVertexAttribPointer(0, 3, GL_FLOAT, false, GLsizeiptr(SizeInBytes()), BUFFER_OFFSET(offset));
			offset += 3 * sizeof(float);
			glVertexAttribPointer(1, 3, GL_FLOAT, false, GLsizeiptr(SizeInBytes()), BUFFER_OFFSET(offset));
			offset += 3 * sizeof(float);
			assert(offset == SizeInBytes());
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
		}
	}

	namespace PN
	{
		size_t Layout::SizeInBytes() {
			return (3 + 3) * sizeof(float);
		}

		void Layout::SetupLayout() {
			unsigned int offset = 0;
			glVertexAttribPointer(0, 3, GL_FLOAT, false, GLsizeiptr(SizeInBytes()), BUFFER_OFFSET(offset));
			offset += 3 * sizeof(float);
			glVertexAttribPointer(1, 3, GL_FLOAT, false, GLsizeiptr(SizeInBytes()), BUFFER_OFFSET(offset));
			offset += 3 * sizeof(float);
			assert(offset == SizeInBytes());
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
		}
	}

	namespace PTN
	{
		size_t Layout::SizeInBytes() {
			return (3 + 3 + 2) * sizeof(float);
		}

		void Layout::SetupLayout() {
			unsigned int offset = 0;
			glVertexAttribPointer(0, 3, GL_FLOAT, false, GLsizeiptr(SizeInBytes()), BUFFER_OFFSET(offset));
			offset += 3 * sizeof(float);
			glVertexAttribPointer(1, 3, GL_FLOAT, false, GLsizeiptr(SizeInBytes()), BUFFER_OFFSET(offset));
			offset += 3 * sizeof(float);
			glVertexAttribPointer(2, 2, GL_FLOAT, false, GLsizeiptr(SizeInBytes()), BUFFER_OFFSET(offset));
			offset += 2 * sizeof(float);
			assert(offset == SizeInBytes());
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
		}
	}

}
