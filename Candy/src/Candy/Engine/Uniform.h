/*
 * Uniform.h
 *
 *  Created on: 12.02.2009
 *      Author: david
 */
#pragma once

#include "../Candy_Dll.h"
#include <Candy/Tools/LinAlg.h>
#include <string>
#include <vector>

namespace Candy {

class Uniform
{
public:
	Uniform(const std::string& name)
	: _name(name) {
	}

	virtual ~Uniform() {}

	const std::string& name() const {
		return _name;
	}

	/** Sets the uniform - must be overrided by derived classes */
	virtual void Set(int location) const = 0;

private:
	std::string _name;
};

class CANDY_API Uniform1i
: public Uniform
{
public:
	Uniform1i(const std::string& name, int a)
	: Uniform(name) {
		var = a;
	}

	virtual ~Uniform1i() {}

	void Set(int location) const;

public:
	int var;

};

class CANDY_API Uniform1f
: public Uniform
{
public:
	Uniform1f(const std::string& name, float a)
	: Uniform(name) {
		var = a;
	}

	virtual ~Uniform1f() {}

	void Set(int location) const;

public:
	float var;

};

class CANDY_API Uniform2f
: public Uniform
{
public:
	Uniform2f(const std::string& name, float a, float b)
	: Uniform(name) {
		Variable1 = a;
		Variable2 = b;
	}

	virtual ~Uniform2f() {}

	void Set(int location) const;

public:
	float Variable1;
	float Variable2;

};

class CANDY_API Uniform3f
: public Uniform
{
public:
	Uniform3f(const std::string& name)
	: Uniform(name) {
		Change(0.0f, 0.0f, 0.0f);
	}

	Uniform3f(const std::string& name, float a, float b, float c)
	: Uniform(name) {
		Change(a, b, c);
	}

	virtual ~Uniform3f() {}

	void Change(float a1, float a2, float a3) {
		Variable1 = a1;
		Variable2 = a2;
		Variable3 = a3;
	}

	void Change(const Vec3f& a) {
		Variable1 = a.x();
		Variable2 = a.y();
		Variable3 = a.z();
	}

	void Set(int location) const;

public:
	float Variable1;
	float Variable2;
	float Variable3;

};

class CANDY_API Uniform4f
: public Uniform
{
public:
	Uniform4f(const std::string& name)
	: Uniform(name) {
		Change(0.0f, 0.0f, 0.0f, 0.0f);
	}

	Uniform4f(const std::string& name, float a, float b, float c, float d)
	: Uniform(name) {
		Change(a, b, c, d);
	}

	virtual ~Uniform4f() {}

	void Set(int location) const;

	void Change(float a1, float a2, float a3, float a4) {
		Variable1 = a1;
		Variable2 = a2;
		Variable3 = a3;
		Variable4 = a4;
	}

	void Change(const Vec3f& a, float w) {
		Variable1 = a.x();
		Variable2 = a.y();
		Variable3 = a.z();
		Variable4 = w;
	}

	void Change(const Vec4f& a) {
		Variable1 = a.x();
		Variable2 = a.y();
		Variable3 = a.z();
		Variable4 = a.w();
	}

public:
	float Variable1;
	float Variable2;
	float Variable3;
	float Variable4;

};

class CANDY_API UniformMat4f
: public Uniform
{
public:
	UniformMat4f(const std::string& name)
	: Uniform(name), is_row_major_(false) {
	}

	virtual ~UniformMat4f() {}

	void Set(int location) const;

	/*void Change(float data[16]) {
		for(int i=0; i<16; i++) {
			data_[i] = data[i];
		}
	}*/

	void Change(const Mat4f& a);

	const Mat4f& GetMatrix() const {
		return m_;
	}

	float Get(unsigned int i) const {
		return data_[i];
	}

	const float* Get() const {
		return (float*)data_;
	}

private:
	Mat4f m_;
	float data_[16];
	bool is_row_major_;

};

struct CANDY_API ShaderUniforms
{
	ShaderUniforms();
	void SetProgram(int program);
	void AddToStart(const Uniform* u);
	void AddToTick(const Uniform* u);
	void Remove(const Uniform* u);
	void SetStart();
	void SetTick();
private:
	void UpdateLocations();
private:
	int _program;
	bool _dirty;
	std::vector<const Uniform*> uniforms_start_;
	std::vector<const Uniform*> uniforms_tick_;
	std::vector<int> _locations;
};

}
