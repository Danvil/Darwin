#include "CellRenderling.h"
#include <Candy/Engine/Resources/ResourcePool.h>
#include <Candy/Engine/IDrawable.h>
#include <Candy/Engine/OpenGL.h>
#include <boost/interprocess/sync/scoped_lock.hpp>

using namespace std;

namespace CandyCubes
{
	namespace Rendering
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

		Ptr(Candy::ShaderX) CellRenderling::shader_;
			
		CellRenderling::CellRenderling(Cubes* cubes, Cell* cell)
		: _cubes(cubes), _cell(cell) {
			_is_dirty = true;
			_needs_transfer = false;
		}

		void CellRenderling::PrepareRender()
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

		void CellRenderling::Render() {
			//std::cout << "Rendering cell " << _cell->coordinate() << std::endl;

			TransferMesh();

			if(_cell->HasBorder()) {
				PrepareRender();
				_mesh.Render();
				FinishRender();
			}
		}

		struct AddSideVertex
		{
			static void PickUV(const CoordI& cc_world, CubeType type, int side, int vertex, float& u, float& v) {

				const unsigned int N = 1;
				const float D = 1.0f / float(N) / 4.0f;

				unsigned int tex_index = Appearance::CubeTypeToTextureIndex(type);
				unsigned int tex_u = tex_index & 3;
				unsigned int tex_v = 3 - tex_index / 4;

				int ccw[3] = { cc_world.x, cc_world.y, cc_world.z };

				unsigned int t1, t2, t3;
				Properties::GetTangential(side, t1, t2, t3);

				unsigned int vert = Properties::GetSideVertexData(side, vertex);

				unsigned int du = Properties::GetVertexPositionOffset(vert, t1);
				unsigned int dv = Properties::GetVertexPositionOffset(vert, t2);

				unsigned int ubase = (unsigned int)((ccw[t1] + ccw[t3]) % N);
				unsigned int vbase = (unsigned int)((ccw[t2] + ccw[t3]) % N);

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
					CoordI vertex_coord = Properties::GetVertexCoord(cc_world, side, i);
					Properties::WorldToPosition(vertex_coord, v.px, v.py, v.pz);

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

		void CellRenderling::UpdateMesh()
		{
			if(_cell->IsAppearanceChanged()) {
				if(_cell->HasBorder()) {
					boost::interprocess::scoped_lock<boost::mutex> guard(_mutex);
					PrepareUpdateMesh();
					_cubes->CreateMeshData<Vertex, AddSideVertex, Candy::MeshData<Vertex> >(_cell, _mesh_data);
					FinishUpdateMesh();
				} else {
					_mesh_data.indices()->clear();
					_mesh_data.vertices()->clear();
				}
				_cell->ClearAppearanceDirtyFlag();
				_needs_transfer = true;
			}
		}

		void CellRenderling::TransferMesh()
		{
			if(_needs_transfer) {
				boost::interprocess::scoped_lock<boost::mutex> guard(_mutex);
				_mesh.UpdateMesh(_mesh_data);
				_needs_transfer = false;
			}
		}

	}
}
