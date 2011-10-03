/*
 * Camera.hpp
 *
 *  Created on: 25.09.2011
 *      Author: david
 */

#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include "../Candy_Dll.h"
#include "Uniform.h"
#include <Candy/Tools/LinAlg.h>
#include <Candy/Tools/Ptr.h>
#include <stack>

namespace Candy {

class ShaderX;

/** Camera
 * M = proj * view^{-1} * model_1 * ... * model_n
 */
class CANDY_API Camera
{
public:
	Camera();

	const Eigen::Affine3f& GetView() const {
		return view_;
	}

	const Eigen::Matrix4f& GetViewMatrix() const {
		return view_matrix_;
	}

	void SetView(const Eigen::Affine3f& m);

	void SetPosition(const Eigen::Vector3f& p) {
		Eigen::Affine3f v;
		v.linear() = view_.linear();
		v.translation() = p;
		SetView(v);
	}

	Eigen::Vector3f GetPosition() const {
		return view_.translation();
	}

	Eigen::Vector3f GetForwardDirection() const {
		return -view_.linear().col(2);
	}

	Eigen::Vector3f GetSidewardsDirection() const {
		return view_.linear().col(0);
	}

	Eigen::Vector3f GetUpDirection() const {
		return view_.linear().col(1);
	}

	void CorrectUp(const Eigen::Vector3f& up_new);

	const Eigen::Matrix4f& GetProjectionMatrix() const {
		return projection_matrix_;
	}

//	void SetProjection(const Eigen::Matrix4f& m) {
//		view_matrix_ = m;
//	}

	void SetViewport(unsigned int w, unsigned int h) {
		width_ = w;
		height_ = h;
	}

	void PixelRay(unsigned int x, unsigned int y, Vec3f& a, Vec3f& u) {
		LinAlg::ComputePixelRay(projection_matrix_, view_, x, y, width_, height_, a, u);
	}

	bool IsVisibleByCamera(const Vec3f& pos);

	float DistanceToEye(const Vec3f& pos);

	void Apply();

	void Push(const Eigen::Matrix4f& m) {
		Eigen::Matrix4f current;
		if(model_view_stack_.empty()) {
			current = m;
		}
		else {
			current = model_view_stack_.top() * m;
		}
		model_view_stack_.push(current);
		u_view_->Change(current);
	}

	void Push() {
		model_view_stack_.push(model_view_stack_.top());
	}

	void Pop() {
		model_view_stack_.pop();
	}

	void AddCameraShaders(const Ptr(ShaderX)& shader);

private:
	unsigned int width_, height_;
	Eigen::Affine3f view_;
	Eigen::Matrix4f view_matrix_;
	Eigen::Matrix4f view_matrix_inv_;
	std::stack<Eigen::Matrix4f> model_view_stack_;
	Eigen::Matrix4f projection_matrix_;
	Ptr(UniformMat4f) u_proj_;
	Ptr(UniformMat4f) u_view_;

};

}

#endif
