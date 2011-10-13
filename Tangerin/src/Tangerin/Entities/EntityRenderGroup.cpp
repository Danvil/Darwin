/*
 * EntityRenderGroup.cpp
 *
 *  Created on: Jun 2, 2011
 *      Author: david
 */

#include "EntityRenderGroup.h"
#include <Candy/Cubes/Appearance.h>
#include <Candy/Cubes/Common.h>
#include <Candy/Engine/Resources.h>
#include <Candy/Tools/Random.h>
#include <Candy/Tools/MoreStd.h>
#include <Candy/Tools/Benchmark.h>
#include <boost/bind.hpp>

#undef GetCurrentTime

namespace Tangerin {

EntityRenderGroup::EntityRenderGroup(const Ptr(EntityRenderInfo)& eri)
{
	render_info_ = eri;
	// load mesh or mesh animation
//	if(is_dynamic_) {
//		visual_ = Candy::ResourcePool::Singleton->Get(
//			Ptr(Candy::MeshAnimId)(new Candy::MeshAnimId(Candy::MeshPN, fn_mesh)));
//	}
//	else {
	visual_ = Ptr(Candy::IDrawable)(Candy::ResourcePool::Singleton->Get(
			Ptr(Candy::MeshId)(new Candy::MeshId(Candy::MeshPTN, render_info_->fn_mesh_))),
			boost::detail::dynamic_cast_tag());
//	}
	// callback for applying creep variables to the shader for rendering
	on_prepare_item_ = boost::bind(&EntityRenderGroup::PrepareItem, this, _1, _2);
}

void EntityRenderGroup::Add(const Ptr(Entity)& a)
{
	data_.push_back(a);
}

void EntityRenderGroup::Remove(const Ptr(Entity)& a)
{
	MoreStd::remove_first(data_, a);
}

void EntityRenderGroup::InitializeShader()
{
	if(render_info_->fn_texture_emit_.empty()) {
		shader_ = Candy::CreatePtnShader("shader/Creature_Textured.vert", "shader/Creature_Tex_AO.frag", render_info_->fn_texture_ao_, "uTextureAmbient");
	}
	else {
		shader_ = Candy::CreatePtnShader("shader/Creature_Textured.vert", "shader/Creature_Tex_AO_Emit.frag", render_info_->fn_texture_ao_, "uTextureAmbient");
		Ptr(Candy::TextureId) ti(new Candy::TextureId(render_info_->fn_texture_emit_));
		shader_->AddTexture("uTextureEmit", Candy::ResourcePool::Singleton->Get(ti));
	}
	// position of sun in the sky
	u_sun_position_ = new Candy::Uniform3f("uSunPosition");
	u_sun_position_->Change(Appearance::SunPosition);
	shader_->AddUniformStart(u_sun_position_);
	// creep color multiplied with ambient color multiplied with ambient strength
	u_object_light_ambient_ = new Candy::Uniform4f("uObjectLightAmbient");
	shader_->AddUniformTick(u_object_light_ambient_);
	// creep color multiplied with sun color multiplied with sun strength
	u_object_light_sun_ = new Candy::Uniform4f("uObjectLightSun");
	shader_->AddUniformTick(u_object_light_sun_);
}

bool EntityRenderGroup::PrepareItem(size_t i, const Ptr(Candy::ShaderX)& shader)
{
	// FIXME check visibility
	//	Candy::IDrawable::sCamera-> ->u_view_->Change(base);
	//	return Candy::IDrawable::view->IsVisibleByCamera(at(i)->phy().position());

	Ptr(Entity) c = at(i);
//	// 3D pose
//	Eigen::Affine3f t = Eigen::Translation3f(c->position())
//	 * Eigen::AngleAxisf(c->direction_angle(), Eigen::Vector3f::UnitZ());
//	Mat4f Tnode = t.matrix();
//	float sizex = 1.0f;//a.dyn().size.x() / cWorldScale;
//	float sizey = 1.0f;//a.dyn().size.y() / cWorldScale;
//	float sizez = 1.0f;//a.dyn().size.z() / cWorldScale;
//	Tnode(0,0) *= sizex;
//	Tnode(0,1) *= sizey;
//	Tnode(0,2) *= sizez;
//	Tnode(1,0) *= sizex;
//	Tnode(1,1) *= sizey;
//	Tnode(1,2) *= sizez;
//	Tnode(2,0) *= sizex;
//	Tnode(2,1) *= sizey;
//	Tnode(2,2) *= sizez;
	Candy::IDrawable::sCamera->Change(c->getPoseMatrix());
	// color
//	Vec3f color = Vec3f(1,1,1);
//	// ambient light
//	//const Vec3f cAmbientColor(0.3f, 0.3f, 0.35f);
//	Vec3f object_light_ambient = (a.dyn().light_ambient_ * color).array() * Appearance::AmbientColor.array();
//	u_object_light_ambient_->Change(object_light_ambient(0), object_light_ambient(1), object_light_ambient(2), 1.0f);
	u_object_light_ambient_->Change(c->getLightAmbient(), 1.0f);
//	// sun light
//	Vec3f object_light_sun = (a.dyn().light_sun_ * color).array() * Appearance::SunColor.array();
//	u_object_light_sun_->Change(object_light_sun(0), object_light_sun(1), object_light_sun(2), 1.0f);
	u_object_light_sun_->Change(c->getLightSun(), 1.0f);
//	// animation parameter for animated meshes
//	if(is_dynamic_) {
//		float animation_time = 0.0f;
//		if(a.sim().is_lifetime_animation) {
//			animation_time = a.dyn().age / a.sim().lifespan;
//		}
//		else {
//			animation_time = a.dyn().anim_v_ * (a.dyn().anim_dt_ + Candy::IDrawable::GetCurrentTime());
//		}
//		dynamic_cast<Candy::Animation*>(visual_.get())->animation_time_ = animation_time;
//	}

	return true;
}

}

