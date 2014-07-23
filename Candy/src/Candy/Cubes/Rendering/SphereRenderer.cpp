/*
 * SphereRenderer.cpp
 *
 *  Created on: 23.09.2011
 *      Author: david
 */

#include "SphereRenderer.hpp"
#include "../Appearance.h"
#include <Candy/Engine/Resources.h>

namespace Candy {
namespace Cubes {
namespace Rendering {

void SphereRenderer::Render()
{
	if(!mesh_) {
		Ptr(Candy::MeshId) mesh_info(new Candy::MeshId(Candy::MeshPN, "sphere.obj"));
		Ptr(Candy::Ressource) res = Candy::ResourcePool::Singleton->Get(mesh_info);
		mesh_ = std::dynamic_pointer_cast<Candy::PN::Mesh>(res);
	}
	if(!shader_) {
		shader_.reset(new Candy::ShaderX());
		std::vector<std::string> attributes;
		attributes.push_back("in_position");
		attributes.push_back("in_normal");
		std::vector<std::string> bindings;
		bindings.push_back("OutFragColor");
		Ptr(Candy::ShaderInfo) info = Candy::ResourcePool::Singleton->Load(
			"SimplePN.vert",
			"ColorAndSun.frag",
			attributes, bindings
		);
		shader_->SetProgram(Candy::ResourcePool::Singleton->Get(info));
		Candy::IDrawable::sCamera->AddCameraShaders(shader_);

		Candy::Uniform4f* ambient = new Candy::Uniform4f("uAmbientColor");
		ambient->Change(Appearance::AmbientColor, 1.0f);
		shader_->AddUniformStart(ambient);

		Candy::Uniform4f* sun_color = new Candy::Uniform4f("uSunColor");
		sun_color->Change(Appearance::SunColor, 1.0f);
		shader_->AddUniformStart(sun_color);

		Candy::Uniform3f* sun_dir = new Candy::Uniform3f("uSunDirection");
		sun_dir->Change(Appearance::SunPosition);
		shader_->AddUniformStart(sun_dir);

		u_color_ = new Candy::Uniform4f("uColor");
		u_color_->Change(1.0f, 0.4f, 0.0f, 1.0f);
		shader_->AddUniformTick(u_color_);
	}
	if(spheres_.size() == 0) {
		return;
	}
	shader_->ApplyStart();
	mesh_->RenderStart();
	std::for_each(spheres_.begin(), spheres_.end(), [&](const std::pair<size_t,Sphere>& pair) {
		const Sphere& sphere = pair.second;
		Eigen::Affine3f t = Eigen::Translation3f(sphere.position)
		 * Eigen::AngleAxisf::Identity();
		Mat4f Tnode = t.matrix();
		float sizex = sphere.radius;
		float sizey = sphere.radius;
		float sizez = sphere.radius;
		Tnode(0,0) *= sizex;
		Tnode(0,1) *= sizey;
		Tnode(0,2) *= sizez;
		Tnode(1,0) *= sizex;
		Tnode(1,1) *= sizey;
		Tnode(1,2) *= sizez;
		Tnode(2,0) *= sizex;
		Tnode(2,1) *= sizey;
		Tnode(2,2) *= sizez;
		Candy::IDrawable::sCamera->Push(Tnode);
		u_color_->Change(sphere.color, 1.0f);
		shader_->ApplyTick();
		mesh_->RenderTick();
		Candy::IDrawable::sCamera->Pop();
	});
	shader_->ApplyStop();
	mesh_->RenderStop();
}


}}}
