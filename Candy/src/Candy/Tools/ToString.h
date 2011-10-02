/*
 * ToString.h
 *
 *  Created on: Jun 9, 2011
 *      Author: david
 */

#ifndef TOSTRING_H_
#define TOSTRING_H_

#include <iomanip>
#include <string>
#include <sstream>

template<typename T>
std::string to_string(const T& t) {
	std::stringstream ss;
	ss << t;
	return ss.str();
}

inline std::string to_string(float t, size_t digits=5, size_t padding=0) {
	std::stringstream ss;
	ss << std::setprecision(digits);
	ss << std::setw(padding);
	ss << t;
	return ss.str();
}

#endif
