#pragma once

#include <Candy/Candy_Dll.h>
#include <Eigen/Core>
#include <Eigen/Geometry>

typedef Eigen::Vector3f Vec3f;
typedef Eigen::Vector4f Vec4f;
typedef Eigen::Matrix3f Mat3f;
typedef Eigen::Matrix4f Mat4f;

namespace LinAlg
{
	/** Creates a 90° projection matrix */
	CANDY_API Mat4f ProjectionMatrix(float aspect, float z_near, float z_far);

	/** Creates a look at view matrix */
	CANDY_API Eigen::Affine3f LookAt(const Vec3f& eye, const Vec3f& target, const Vec3f& up);

	/** Pans the current view matrix */
	CANDY_API Eigen::Affine3f Pan(const Eigen::Affine3f& m, float dx, float dy, float dz);

	CANDY_API Eigen::Affine3f RotateAroundTarget(const Eigen::Affine3f& m, float ax, float ay);

	CANDY_API Eigen::Affine3f Zoom(const Eigen::Affine3f& m, float a);

	/** Computes square distance between a point and a ray
	 * Assumes, that u is normalized!
	 */
	inline float PointRayDistance2(const Vec3f& x, const Vec3f& a, const Vec3f& u, float& l) {
		Vec3f d = x - a;
		l = u.dot(d);
		Vec3f h = d - l*u;
		return h.squaredNorm();
	}

	inline float PointRayDistance2(const Vec3f& x, const Vec3f& a, const Vec3f& u) {
		float l;
		return PointRayDistance2(x, a, u, l);
	}

	CANDY_API void ComputePixelRay(const Mat4f& proj, const Eigen::Affine3f& view, int x, int y, int w, int h, Vec3f& a, Vec3f& u);

}
