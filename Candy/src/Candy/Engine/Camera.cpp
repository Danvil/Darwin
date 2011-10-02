/*
 * Camera.cpp
 *
 *  Created on: 25.09.2011
 *      Author: david
 */

#include "Camera.hpp"
#include "IDrawable.h"
#include "Shader.h"
#include <GL/glew.h>

namespace Candy {

Camera::Camera()
{
	u_proj_.reset(new Candy::UniformMat4f("uProjectionMatrix"));
	u_view_.reset(new Candy::UniformMat4f("uModelviewMatrix"));
}

void Camera::SetView(const Eigen::Affine3f& m)
{
	view_ = m;
	view_matrix_ = view_.matrix();
	view_matrix_inv_ = view_.inverse().matrix();
}

bool Camera::IsVisibleByCamera(const Vec3f& pos)
{
	Mat4f u = projection_matrix_ * view_matrix_inv_;
	Vec4f screen = u * Vec4f(pos(0), pos(1), pos(2), 1);
	float w_inv = 1.0f / screen(3);
	float x = screen(0) * w_inv;
	float y = screen(1) * w_inv;
	float z = screen(2) * w_inv;
	return (-1 <= x && x <= +1) && (-1 <= y && y <= +1) && (-1 <= z && z <= +1);
}

float Camera::DistanceToEye(const Vec3f& pos_world)
{
	Vec4f pw4;
	pw4 << pos_world , 1;
	Vec4f pos_cam = view_matrix_inv_ * pw4;
	pos_cam(3) = 0;
	return pos_cam.norm();
}

void Camera::Apply()
{
	// change viewport to full window area
	glViewport(0, 0, width_, height_);
	// change projection matrix
	projection_matrix_ = LinAlg::ProjectionMatrix(float(height_) / float(width_), 0.1f, 1000.0f);
	u_proj_->Change(projection_matrix_);
	// set matrix uniforms
	model_view_stack_ = std::stack<Eigen::Matrix4f>();
	Push(view_matrix_inv_); // must use inverse of camera matrix!
}

void Camera::AddCameraShaders(const Ptr(ShaderX)& shader)
{
	shader->AddUniformStart(u_proj_.get());
	shader->AddUniformTick(u_view_.get());
}

}
