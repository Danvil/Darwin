/*
 * SphereRenderer.hpp
 *
 *  Created on: 23.09.2011
 *      Author: david
 */

#ifndef SPHERERENDERER_HPP_
#define SPHERERENDERER_HPP_

#include <Candy/Engine/IDrawable.h>
#include <Candy/Engine/Mesh.h>
#include <Candy/Engine/Shader.h>
#include <Candy/Tools/LinAlg.h>
#include <map>
#include "stddef.h"

namespace Candy {
namespace Cubes {
namespace Rendering {

class SphereRenderer
: public IDrawable
{
public:
	struct Sphere {
		Vec3f position;
		Vec3f color;
		float radius;
	};

	SphereRenderer()
	: next_(0) {}

	size_t Add(const Sphere& sphere) {
		spheres_[next_] = sphere;
		return next_++;
	}

	Sphere& Get(size_t id) {
		// TODO check that exists
		return spheres_[id];
	}

	void Remove(size_t id) {
		spheres_.erase(id);
	}

	void Render();

private:
	std::map<size_t, Sphere> spheres_;
	size_t next_;
	Ptr(Candy::PN::Mesh) mesh_;
	Ptr(Candy::ShaderX) shader_;
	Candy::Uniform4f* u_color_;
};

}}}

#endif
