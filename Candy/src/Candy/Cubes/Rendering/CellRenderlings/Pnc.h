#pragma once

#include "TriangleVerticeRenderling.h"
#include "../../Common.h"
#include "../../Cube.h"
#include <Candy/Engine/Mesh.h>
#include <vector>

namespace Candy { class ShaderX; }

namespace CellRenderlings
{
	namespace PNC
	{

		class PncLayout
		: public Candy::IMeshVertexLayout
		{
		public:
			size_t SizeInBytes();
			void SetupLayout();
		};

		struct PncVertex
		{
			float px, py, pz;
			float nx, ny, nz;
			float cr, cg, cb;
		};

		typedef Candy::SpecializedMesh<PncVertex, PncLayout> PncMesh;

		struct PncAddSideVertex
		{
			static void Add(std::vector<PncVertex>* vertices, const CoordI& cc_world, CubeType cube, CubeSideData* data, int side) {
				for(int i = 0; i < 4; i++) {
					PncVertex v;
					CoordI vertex_coord = Properties::GetVertexCoord(cc_world, side, i);
					Properties::WorldToPosition(vertex_coord, v.px, v.py, v.pz);
					Properties::GetSideNormal(cc_world, side, v.nx, v.ny, v.nz);
					Vec3f col = data->object_color;
					v.cr = col.x();
					v.cg = col.y();
					v.cb = col.z();
					vertices->push_back(v);
				}
			}
		};

		struct PncTraits
		{
			typedef PncLayout Layout;
			typedef PncVertex Vertex;
			typedef PncMesh Mesh;
			typedef PncAddSideVertex AddSideVertex;
		};

		class PncRenderling
			: public TriangleVerticeRenderling<PncTraits>
		{
		public:
			PncRenderling(Cubes* cubes, Cell* cell);

			virtual ~PncRenderling();

			void PrepareUpdateMesh();

			void FinishUpdateMesh();

			void PrepareRender();

			void FinishRender();

		private:
			static Ptr(Candy::ShaderX) shader_;

		};

	}
}
