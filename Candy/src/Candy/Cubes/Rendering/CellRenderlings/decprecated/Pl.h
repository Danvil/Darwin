#pragma once

#include "TriangleVerticeRenderling.h"
#include "../../Common.h"
#include "../../Cubes.h"
#include <Candy/Mesh.h>
#include <vector>

namespace Candy { class ShaderProgram; }

namespace CellRenderlings
{
	namespace PL
	{

		class PlLayout
		: public Candy::IMeshVertexLayout
		{
		public:
			size_t SizeInBytes();
			void SetupLayout();
		};

		struct PlVertex
		{
			float px, py, pz; // position
			float ambient; // ambient lighting
			float scenery_r, scenery_g, scenery_b; // scenery lighting color
		};

		typedef Candy::SpecializedMesh<PlVertex, PlLayout> PlMesh;

		struct PlAddSideVertex
		{
			static void Add(std::vector<PlVertex>* vertices, const CoordI& cc_world, const Cube& cube, int side) {
				for(int i = 0; i < 4; i++) {
					PlVertex v;
					Properties::GetVertexPosition(cc_world, side, i, v.px, v.py, v.pz);
					const CubeSideLightData& light = cube.data().lighting[side];
					v.ambient = light.ambient;
					v.scenery_r = light.scenery.x;
					v.scenery_g = light.scenery.y;
					v.scenery_b = light.scenery.z;
					vertices->push_back(v);
				}
			}
		};

		struct PlTraits
		{
			typedef PlLayout Layout;
			typedef PlVertex Vertex;
			typedef PlMesh Mesh;
			typedef PlAddSideVertex AddSideVertex;
		};

		class PlRenderling
			: public TriangleVerticeRenderling<PlTraits>
		{
		public:
			PlRenderling(Cubes* cubes, Cell* cell);

			virtual ~PlRenderling();

			void PrepareUpdateMesh();

			void FinishUpdateMesh();

			void PrepareRender();

			void FinishRender();

		private:
			Candy::ShaderProgram* shader_;

		};

	}
}
