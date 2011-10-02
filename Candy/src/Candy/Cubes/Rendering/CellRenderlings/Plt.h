#pragma once

#include "../../../Candy_Dll.h"
#include "TriangleVerticeRenderling.h"
#include "../../Common.h"
#include "../../Cubes.h"
#include <Candy/Engine/Resources.h>
#include <Candy/Cubes/Appearance.h>
#include <Candy/Engine/Mesh.h>
#include <Candy/Tools/Perlin.h>
#include <vector>

namespace Candy { class ShaderProgram; }
namespace Candy { class Uniform1i; }
namespace Candy { class Texture2; }

namespace CellRenderlings
{
	namespace PLT
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

		struct AddSideVertex
		{
			static void PickUV(const CoordI& cc_world, CubeType type, int side, int vertex, float& u, float& v) {

				const unsigned int N = 1;
				const float D = 1.0f / float(N) / 4.0f;

				int tex_index = Appearance::CubeTypeToTextureIndex(type);
				int tex_u = tex_index & 3;
				int tex_v = 3 - tex_index / 4;

				int ccw[3] = { cc_world.x, cc_world.y, cc_world.z };

				int t1 = Geometry::Tangential[side][0];
				int t2 = Geometry::Tangential[side][1];
				int t3 = Geometry::Tangential[side][2];

				int vert = Geometry::SideVertexIndices[side][vertex];

				int du = Geometry::VertexPositionOffset[vert][t1];
				int dv = Geometry::VertexPositionOffset[vert][t2];

				int ubase = (ccw[t1] + ccw[t3]) % N;
				int vbase = (ccw[t2] + ccw[t3]) % N;

				u = D * float(tex_u + ubase + du);
				v = D * float(tex_v + vbase + dv);

				//} else {
				//	switch(vertex) {
				//	case 0: u = 0; v = 0; break;
				//	case 1: u = 0; v = 1; break;
				//	case 2: u = 1; v = 1; break;
				//	case 3: u = 1; v = 0; break;
				//	}
				//}
			}

			static void Add(std::vector<Vertex>* vertices, const CoordI& cc_world, CubeType type, CubeSideData* data, int side) {
				const CubeSideLightData& light = data->lighting;
				for(int i = 0; i < 4; i++) {
					Vertex v;
					Geometry::GetVertexPosition(cc_world, side, i, v.px, v.py, v.pz);

//					// randomly resizes cubes for a more natural look
//					float dx = 0.257f * Perlin::NoiseMPScl(v.px+0.23f, v.py, v.pz, 10.0f);
//					float dy = 0.257f * Perlin::NoiseMPScl(v.px, v.py+0.45f, v.pz, 10.0f);
//					v.px += dx;
//					v.py += dy;
//					if(type == CubeTypes::Water) {
						// water is flat however ...
//						v.pz -= 0.15f;
//					}
//					else {
//						float dz = 0.257f * Perlin::NoiseMPScl(v.px, v.py, v.pz+0.34f, 10.0f);
//						v.pz += dz;
//					}

					v.ambient = light.ambient > 1.0f ? 1.0f : light.ambient;
					v.sun = light.sun > 1.0f ? 1.0f : light.sun;
					v.scenery_r = light.scenery(0) > 1.0f ? 1.0f : light.scenery(0);
					v.scenery_g = light.scenery(1) > 1.0f ? 1.0f : light.scenery(1);
					v.scenery_b = light.scenery(2) > 1.0f ? 1.0f : light.scenery(2);
					PickUV(cc_world, type, side, i, v.u, v.v);
					vertices->push_back(v);
				}
			}
		};

		struct Traits
		{
			typedef ::CellRenderlings::PLT::Layout Layout;
			typedef ::CellRenderlings::PLT::Vertex Vertex;
			typedef ::CellRenderlings::PLT::Mesh Mesh;
			typedef ::CellRenderlings::PLT::AddSideVertex AddSideVertex;
		};

		class CANDY_API Renderling
			: public TriangleVerticeRenderling<Traits>
		{
		public:
			Renderling(Cubes* cubes, Cell* cell);

			virtual ~Renderling();

			void PrepareUpdateMesh();

			void FinishUpdateMesh();

			void PrepareRender();

			void FinishRender();

		private:
			static Ptr(Candy::ShaderX) shader_;

		};

	}
}
