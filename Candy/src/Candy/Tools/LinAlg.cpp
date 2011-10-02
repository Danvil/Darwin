#include "LinAlg.h"
#include <Eigen/Eigen>
#include <stdexcept>
#include <iostream>

namespace LinAlg
{
	Mat4f ProjectionMatrix(float aspect, float near, float far)
	{
		if(near <= 0) {
			throw std::runtime_error("ProjectionMatrix: Near must be greater than 0!");
		}
		if(far <= near) {
			throw std::runtime_error("ProjectionMatrix: Far must be greater than near!");
		}
		// change projection matrix (90° fov)
		const float cRight = near; // this will give 90° opening angle
		const float cTop = cRight * aspect; //float(height) / float(width); // consider aspect ratio
		Mat4f proj;
		// see http://www.songho.ca/opengl/gl_projectionmatrix.html
		proj <<
			near/cRight, 0, 0, 0,
			0, near/cTop, 0, 0,
			0, 0, -(far+near)/(far-near), -2*far*near/(far-near),
			0, 0, -1, 0;
		return proj;
	}

	Eigen::Affine3f LookAt(const Vec3f& eye, const Vec3f& target, const Vec3f& up_given)
	{
		if(up_given.norm() == 0) {
			throw std::runtime_error("LookAt: Up vector must not be 0!");
		}
		//------------------
		Vec3f forward = target - eye;
		if(forward.norm() == 0) {
			throw std::runtime_error("LookAt: Up vector must not be collinear to target-eye!");
		}
		forward.normalize();
		//------------------
		//Side = forward x up
		Vec3f side = forward.cross(up_given);
		//side.normalize();
		//------------------
		//Recompute up as: up = side x forward
 		Vec3f up = side.cross(forward);
		//up.normalize();
		//------------------
		Eigen::Matrix3f r;
		r.col(0) = side;
		r.col(1) = up;
		r.col(2) = -forward;
		Eigen::Affine3f m;
		m.linear() = r;
		m.translation() = eye;
		/*Mat4f m;
		m <<
			side(0), up(0), -forward(0), eyePosition3D(0),
			side(1), up(1), -forward(1), eyePosition3D(1),
			side(2), up(2), -forward(2), eyePosition3D(2),
			0, 0, 0, 1;*/
		//------------------
		return m;
	}

	Eigen::Affine3f Pan(const Eigen::Affine3f& m, float dx, float dy, float dz)
	{
		Vec3f p = m.linear() * Vec3f(dx, dy, dz);
		Eigen::Affine3f n = m;
		n.translation() += p;
		return n;
	}

	Eigen::Affine3f RotateAroundTarget(const Eigen::Affine3f& m, float ax, float ay)
	{
		return m
			* Eigen::AngleAxisf(ax, Eigen::Vector3f::UnitY())
			* Eigen::AngleAxisf(ay, Eigen::Vector3f::UnitX());
/*		Vec3f e = camera->getEye();
		Vec3f o = camera->getTarget();
		Vec3f x = o - e;
		Vec3f right = camera->getRight();
		Mat3f Rz = ctLinAlg::TransformationFactory::RotationVector(camera->getUp(), -amountX);
		Mat3f Ru = ctLinAlg::TransformationFactory::RotationVector(right, -amountY);
		e = o - Rz*Ru*x;

		this->eye = eye;
		view = target - eye;
		update();
		Normalize(view);
		right = Cross(view, upOriginal);
		Normalize(right);
		up = ctLinAlg::Cross(right, view);
		*/
		return m;
	}

	Eigen::Affine3f Zoom(const Eigen::Affine3f& m, float a)
	{
		Eigen::Affine3f n = m;
		n.translation() += a * n.linear().col(2);
		return n;
	}

	void ComputePixelRay(const Mat4f& proj, const Eigen::Affine3f& view, int x, int y, int w, int h, Vec3f& a, Vec3f& u)
	{
		float px = 2.0f*float(x)/float(w) - 1.0f;
		float py = 2.0f*float(h - y)/float(h) - 1.0f;
		Vec4f q = proj.householderQr().solve(Vec4f(px, py, -1, +1));
		a = view * Vec3f(q.x(), q.y(), q.z());
		u = a - view.translation();
		a = view.translation();
	}

}