/*
 * Common.h
 *
 *  Created on: 04.06.2011
 *      Author: david
 */

#ifndef STARLORD_SLGUI_COMMON_H_
#define STARLORD_SLGUI_COMMON_H_
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "../Candy_Dll.h"
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <memory>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

typedef int KeyboardModifiers;

typedef int MouseButton;

typedef Eigen::Vector2f Point2;

typedef Eigen::Vector4f Colorf;

#define PTR(X) std::shared_ptr<X>

template<typename A, typename B>
bool DanvilIsOfType(const PTR(B)& b) {
	return dynamic_cast<A*>(b.get()) != 0;
}

template<typename A, typename B>
PTR(A) DanvilConvert(const PTR(B)& b) {
	return std::dynamic_pointer_cast<A>(b);
}

template<typename T>
void Danvilifmin(T& a, const T& val) {
	if(val < a) {
		a = val;
	}
}

template<typename T>
void Danvilifmax(T& a, const T& val) {
	if(val > a) {
		a = val;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif
