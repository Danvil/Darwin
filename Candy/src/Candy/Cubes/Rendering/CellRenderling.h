#pragma once

#include "CellRenderlingInterface.h"
#include <Candy/Candy_Dll.h>
#include <Candy/Cubes/Common.h>
#include <Candy/Cubes/Cubes.h>
#include <Candy/Engine/Resources.h>
#include <Candy/Cubes/Appearance.h>
#include <Candy/Engine/Mesh.h>
#include <Candy/Engine/Uniform.h>
#include <Candy/Tools/Perlin.h>
#include <GL/glew.h>
#include <boost/shared_array.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <vector>

namespace Candy { class ShaderProgram; }
namespace Candy { class Uniform1i; }
namespace Candy { class Texture2; }

namespace CandyCubes
{
	namespace Rendering
	{
		class CANDY_API Layout
		: public Candy::IMeshVertexLayout
		{
		public:
			size_t SizeInBytes();
			void SetupLayout();
		};

		struct Vertex
		{
			float px, py, pz; // position
			float u, v;
			float ambient; // ambient lighting
			float sun; // sun lighting
			float scenery_r, scenery_g, scenery_b; // scenery lighting color
		};

		typedef Candy::SpecializedMesh<Vertex, Layout> Mesh;

		class CellRenderling
		: public CellRenderlingInterface
		{
		public:
			CellRenderling(Cubes* cubes, Cell* cell);

			virtual ~CellRenderling() {}

		protected:
			void PrepareUpdateMesh() {}

			void FinishUpdateMesh() {}

			void PrepareRender();

			void FinishRender() {}

			Cell* GetCell() const {
				return _cell;
			}

		public:
			void Render();

			bool UpdateMesh();

			void TransferMesh();

		private:
			static Ptr(Candy::ShaderX) shader_;

			Cubes* _cubes;

			Cell* _cell;

			bool _is_dirty;

			bool _needs_transfer;

			Mesh _mesh;

			Candy::MeshData<Vertex> _mesh_data;

			boost::mutex _mutex;

		};

	}
}
