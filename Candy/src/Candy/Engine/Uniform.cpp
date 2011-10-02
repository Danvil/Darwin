#include "Uniform.h"
#include <GL/glew.h>

namespace Candy {

void Uniform1i::Set(int location) const
{
	glUniform1i(location, var);
}

void Uniform1f::Set(int location) const
{
	glUniform1f(location, var);
}

void Uniform2f::Set(int location) const
{
	glUniform2f(location, Variable1, Variable2);
}

void Uniform3f::Set(int location) const
{
	glUniform3f(location, Variable1, Variable2, Variable3);
}

void Uniform4f::Set(int location) const
{
	glUniform4f(location, Variable1, Variable2, Variable3, Variable4);
}

void UniformMat4f::Set(int location) const
{
	glUniformMatrix4fv(location, 1, is_row_major_, data_);
}

void UniformMat4f::Change(const Eigen::Matrix4f& a) {
	m_ = a;
	for(int i=0; i<16; i++) {
		data_[i] = a(i);
	}
}


ShaderUniforms::ShaderUniforms()
: _program(-1)
{
	_dirty = false;
}

void ShaderUniforms::SetProgram(int program)
{
	if(_program != program) {
		_program = program;
		_dirty = true;
	}
}

void ShaderUniforms::AddToStart(const Uniform* u)
{
	uniforms_start_.push_back(u);
	_dirty = true;
}

void ShaderUniforms::AddToTick(const Uniform* u)
{
	uniforms_tick_.push_back(u);
	_dirty = true;
}

void ShaderUniforms::Remove(const Uniform* u)
{
	// FIXME implement!
	_dirty = true;
}

void ShaderUniforms::SetStart()
{
	if(_program == -1) {
		throw 0; // FIXME 
	}
	// FIXME do not call every time
	if(_dirty) {
		UpdateLocations();
		_dirty = false;
	}
	for(size_t i=0; i<uniforms_start_.size(); i++) {
		uniforms_start_[i]->Set(_locations[i]);
	}
}

void ShaderUniforms::SetTick()
{
	for(size_t i=0; i<uniforms_tick_.size(); i++) {
		uniforms_tick_[i]->Set(_locations[uniforms_start_.size() + i]);
	}
}

void ShaderUniforms::UpdateLocations()
{
	_locations.clear();
	for(size_t i=0; i<uniforms_start_.size(); i++) {
		_locations.push_back(glGetUniformLocation(_program, uniforms_start_[i]->name().c_str()));
	}
	for(size_t i=0; i<uniforms_tick_.size(); i++) {
		_locations.push_back(glGetUniformLocation(_program, uniforms_tick_[i]->name().c_str()));
	}
}

}
