/*
 * DanvilCubes.hpp
 *
 *  Created on: 25.09.2011
 *      Author: david
 */

#ifndef DANVILCUBES_HPP_
#define DANVILCUBES_HPP_

#include "Common.h"
#include "Impl/GroundHeightLookup.h"
#include "Cubes.h"
#include "Cube.h"
#include "Generator.h"
#include <Candy/Engine/IDrawable.h>
#include <Candy/Tools/Ptr.h>

class Cubes;
struct Cell;
namespace Candy { namespace Cubes { namespace Physics { class PhysicsWorld; }}}
namespace Candy { namespace Cubes { namespace Rendering { class SphereRenderer; }}}
class CubesRenderling;
class Background;

namespace Candy
{

	struct GenerationProperties
	{
		Ptr(::Generator) generator_;
		WorldSize world_size_;
		bool build_first_;

		GenerationProperties() : build_first_(false) {}
	};

	class DanvilCubes
	: public Candy::IDrawable
	{
	public:
		DanvilCubes(const GenerationProperties& gen);

		virtual ~DanvilCubes();

		void Tick(float dt);

		void Render();

		Ptr(::Cubes) GetCubes() const {
			return cubes_;
		}

		int GetHeight(int cx, int cy, float z) const {
			return height_lookup_->GetGroundZ(cx, cy, z);
		}

		int GetHeight(const Vec3f& position) const {
			CoordI c_world = Properties::PositionToWorld(position);
			float z = Properties::GetGroundHeightOverNull(position);
			return GetHeight(c_world.x, c_world.y, z);
		}

		Vec3f GetGroundPoint(const Vec3f& position) const {
			int z_soll = GetHeight(position);
			return Properties::PositionWithHeight(position, float(z_soll));
		}

		Vec3f GetNormal(const Vec3f& position) const {
			return Properties::TopSideNormal(position);
		}

		void GetLightAtPosition(const Vec3f& position, float* light_ambient, float* light_sun) const {
			CoordI c_world = Properties::PositionToWorld(position);
			c_world.z = height_lookup_->GetGroundZ(c_world.x, c_world.y, position[2]) - 1;
			CubeSideData* data = cubes_->GetData(c_world, Properties::TopSide());
			if(data == 0) {
				*light_ambient = 0;
				*light_sun = 0;
			}
			else {
				*light_ambient = data->getLighting().ambient;
				*light_sun = data->getLighting().sun;
			}
		}

	private:
		void OnAddCell(Cell* cell);

		void OnUpdateCube(Cell* cell, const CoordI& cw);

	private:
		Ptr(::Cubes) cubes_;

		Ptr(Candy::Cubes::Physics::PhysicsWorld) cubes_physics_;

		Ptr(CubesRenderling) man_;

		Ptr(Candy::Cubes::Rendering::SphereRenderer) sphere_renderer_;

		Background* background_;

		Ptr(GroundHeightLookup) height_lookup_;

	};

}

#endif
