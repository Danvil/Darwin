#pragma once

#include <Candy/Tools/LinAlg.h>
#include <cmath>
#include <algorithm>

namespace Collision
{
	const float EPSILON = 0.0000001f;

	bool IsInside(const Vec3f& box_min, const Vec3f& box_max, const Vec3f& a) {
		return box_min(0) <= a(0) && a(0) <= box_max(0)
			&& box_min(1) <= a(1) && a(1) <= box_max(1)
			&& box_min(2) <= a(2) && a(2) <= box_max(2);
	}

	bool IsIntersecting(const Vec3f& box_min, const Vec3f& box_max, const Vec3f& ray_position, const Vec3f& ray_direction, float& distance) {

		float d = 0.0f;
		float maxValue = float(1e12);

		if(std::abs(ray_direction(0)) < EPSILON) {
			if(ray_position(0) < box_min(0) || ray_position(0) > box_max(0)) {
				distance = 0.0f;
				return false;
			}
		}
		else {
			float inv = 1.0f / ray_direction(0);
			float min = (box_min(0) - ray_position(0)) * inv;
			float max = (box_max(0) - ray_position(0)) * inv;

			if(min > max) {
				float temp = min;
				min = max;
				max = temp;
			}

			d = std::max(min, d);
			maxValue = std::min(max, maxValue);

			if(d > maxValue) {
				distance = 0.0f;
				return false;
			}
		}

		if(std::abs(ray_direction(1)) < EPSILON) {
			if(ray_position(1) < box_min(1) || ray_position(1) > box_max(1)) {
				distance = 0.0f;
				return false;
			}
		}
		else {
			float inv = 1.0f / ray_direction(1);
			float min = (box_min(1) - ray_position(1)) * inv;
			float max = (box_max(1) - ray_position(1)) * inv;

			if(min > max) {
				float temp = min;
				min = max;
				max = temp;
			}

			d = std::max(min, d);
			maxValue = std::min(max, maxValue);

			if(d > maxValue) {
				distance = 0.0f;
				return false;
			}
		}

		if(std::abs(ray_direction(2)) < EPSILON) {
			if(ray_position(2) < box_min(2) || ray_position(2) > box_max(2)) {
				distance = 0.0f;
				return false;
			}
		}
		else {
			float inv = 1.0f / ray_direction(2);
			float min = (box_min(2) - ray_position(2)) * inv;
			float max = (box_max(2) - ray_position(2)) * inv;

			if(min > max) {
				float temp = min;
				min = max;
				max = temp;
			}

			d = std::max(min, d);
			maxValue = std::min(max, maxValue);

			if(d > maxValue) {
				distance = 0.0f;
				return false;
			}
		}
		distance = d;
		return true;
	}
}
