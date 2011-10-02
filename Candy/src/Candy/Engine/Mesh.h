#pragma once

#include "IDrawable.h"
#include "Resources/Resource.h"
#include <boost/shared_array.hpp>
#include <vector>
#include <stdint.h>
#include <cassert>

#define BUFFER_OFFSET(i) ((char *)0 + (i)) // see http://www.opengl.org/wiki/VBO_-_just_examples

namespace Candy {

	class IMeshVertexLayout
	{
	public:
		virtual size_t SizeInBytes() = 0;
		virtual void SetupLayout() = 0;
	};

	class Mesh
		: public Ressource, public IDrawable
	{
	public:
		enum MeshType {
			MeshTypePoints,
			MeshTypeLines,
			MeshTypeTriangles
		};

	public:
		Mesh(IMeshVertexLayout* layout);

		virtual ~Mesh() {}

		void RenderStart();
		void RenderTick();
		void RenderStop();

		void UpdateIndices(uint32_t* indices, size_t count);

		void set_type(MeshType type) { _type = type; }

	protected:
		void UpdateVertices(void* vertices, size_t count);

	private:
		void UpdateVertexData();

		void UpdateIndexData();

		void Bind();

		void Generate();

		void Delete();

	private:
		IMeshVertexLayout *_layout;

		bool MustUpdateVertexData;

		bool MustUpdateIndexData;

		bool has_called_setup_layout;

		MeshType _type;

		uint32_t* _indices;
		size_t _indices_count;

		void* _vertices;
		size_t _vertices_count;

		unsigned int vaoHandle;

		unsigned int vboHandle0, vboHandle1;

	};

	template<class Vertex>
	struct MeshData
	{
		MeshData() {
			_vertices = new std::vector<Vertex>();
			_owns_vertices = true;
			_indices = new std::vector<uint32_t>();
			_owns_indices = true;
		}
		~MeshData() {
			if(_owns_vertices) {
				delete _vertices;
			}
			if(_owns_indices) {
				delete _indices;
			}
		}
		void set_vertices(std::vector<Vertex>* v) {
			if(_owns_vertices) {
				delete _vertices;
				_owns_vertices = false;
			}
			_vertices = v;
		}
		void set_indices(std::vector<uint32_t>* v) {
			if(_owns_indices) {
				delete _indices;
				_owns_indices = false;
			}
			_indices = v;
		}
		std::vector<Vertex>* vertices() const { return _vertices; }
		std::vector<uint32_t>* indices() const { return _indices; }
	private:
		MeshData(const MeshData&);
		MeshData& operator=(const MeshData&);
	private:
		std::vector<Vertex>* _vertices;
		std::vector<uint32_t>* _indices;
		bool _owns_vertices;
		bool _owns_indices;
	};

	template<class Vertex, class Layout>
	class SpecializedMesh
		: public Mesh
	{
	public:
		SpecializedMesh()
		: Mesh(new Layout()) { // FIXME delete layout
		}

		virtual ~SpecializedMesh() {}

		void UpdateVertices(Vertex* vertices, size_t count) {
			Mesh::UpdateVertices(static_cast<void*>(vertices), count);
		}

		void UpdateMesh(const MeshData<Vertex>& data) {
			UpdateVertices(data.vertices()->data(), data.vertices()->size());
			Mesh::UpdateIndices(data.indices()->data(), data.indices()->size());
		}
	};

	namespace P
	{
		class Layout
		: public IMeshVertexLayout
		{
		public:
			size_t SizeInBytes();
			void SetupLayout();
		};

		struct Vertex
		{
			float px, py, pz; // position
			Vertex() {}
			Vertex(float x, float y, float z) : px(x), py(y), pz(z) {}
		};

		typedef SpecializedMesh<Vertex, Layout> Mesh;
		typedef MeshData<Vertex> MeshData;

	}

	/** Position Texture */
	namespace PT
	{
		class Layout
		: public IMeshVertexLayout
		{
		public:
			size_t SizeInBytes();
			void SetupLayout();
		};

		struct Vertex
		{
			float px, py, pz; // position
			float u, v; // texture
			Vertex() {}
			Vertex(
				float npx, float npy, float npz,
				float nu, float nv)
				: px(npx), py(npy), pz(npz), u(nu), v(nv) {}
		};

		typedef SpecializedMesh<Vertex, Layout> Mesh;
		typedef MeshData<Vertex> MeshData;

	}

	/** Position / Normal */
	namespace PN
	{
		class Layout
		: public IMeshVertexLayout
		{
		public:
			size_t SizeInBytes();
			void SetupLayout();
		};

		struct Vertex
		{
			float px, py, pz; // position
			float nx, ny, nz; // normal
			Vertex() {}
			Vertex(
				float npx, float npy, float npz,
				float nnx, float nny, float nnz)
				: px(npx), py(npy), pz(npz), nx(nnx), ny(nny), nz(nnz) {}
		};

		typedef SpecializedMesh<Vertex, Layout> Mesh;
		typedef MeshData<Vertex> MeshData;

	}

	/** Position / Texture / Normal */
	namespace PTN
	{
		class Layout
		: public IMeshVertexLayout
		{
		public:
			size_t SizeInBytes();
			void SetupLayout();
		};

		struct Vertex
		{
			float px, py, pz; // position
			float nx, ny, nz; // normal
			float u, v; // texture
			Vertex() {}
			Vertex(
				float npx, float npy, float npz,
				float nnx, float nny, float nnz,
				float nu, float nv)
				: px(npx), py(npy), pz(npz), nx(nnx), ny(nny), nz(nnz), u(nu), v(nv) {}
		};

		typedef SpecializedMesh<Vertex, Layout> Mesh;
		typedef MeshData<Vertex> MeshData;

	}

}
