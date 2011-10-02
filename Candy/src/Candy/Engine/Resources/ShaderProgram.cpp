#include "ShaderProgram.h"
#include <GL/glew.h>
#include <iostream>
#include <cassert>
#include <stdexcept>
using namespace std;

namespace Candy {

int ShaderProgram::getProgram() const {
	return my_program;
}

int ShaderProgram::getAttributeLocation(const std::string& attribute_name) const {
	return glGetAttribLocation(getProgram(), attribute_name.c_str());
}

ShaderProgram::ShaderProgram(const Ptr(ShaderInfo)& info)
{
	my_program = -1;
	my_geometry_shader = -1;
	my_vertex_shader = -1;
	my_fragment_shader = -1;
	_info = info;
}

/*void ShaderProgram::createTest()
{
	nameVertex = "default";
	nameFragment = "default";
	shaderCodeVertex = "void main() { gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; }";
	shaderCodeFragment = "void main() { gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); }";
}*/

void ShaderProgram::Generate()
{
	//assureGeneratable("ShaderProgram");
	//RAISE_IF_GLEW_NOT_INITIALIZED();

	// Create Program Objects
	my_program = glCreateProgram();

	my_vertex_shader = CreateShader(_info->vertex_src, GL_VERTEX_SHADER);
	glAttachShader(my_program, my_vertex_shader);

	if(_info->geometry_src != "") {
		my_geometry_shader = CreateShader(_info->geometry_src, GL_GEOMETRY_SHADER);
		glAttachShader(my_program, my_geometry_shader);
	}

	my_fragment_shader = CreateShader(_info->fragment_src, GL_FRAGMENT_SHADER);
	glAttachShader(my_program, my_fragment_shader);

	// set the shader attributes
	for(size_t i=0; i<_info->attributes.size(); i++) {
		glBindAttribLocation(my_program, i, _info->attributes[i].c_str());
	}

	// set shader buffer bindings
	for(size_t i=0; i<_info->bindings.size(); i++) {
		glBindFragDataLocation(my_program, i, _info->bindings[i].c_str());
	}

	// Link The Program Object
	glLinkProgram(my_program);
	// test
	if( !IsProgramLinkingSuccessful(my_program) ) {
		cerr << "Error while linking shader" << endl;
		cerr << GetShaderInfoLog(my_program) << endl;
		throw ShaderException();
	}

	set_state(RessourceGenerated);
}

void ShaderProgram::Delete()
{
	//assureFreeable("ShaderProgram");
	if(my_program != -1) {
		glDeleteProgram(my_program);
	}
	if(my_vertex_shader != -1) {
		glDeleteShader(my_vertex_shader);
	}
	if(my_geometry_shader != -1) {
		glDeleteShader(my_geometry_shader);
	}
	if(my_fragment_shader != -1) {
		glDeleteShader(my_fragment_shader);
	}
	//setState(Freed);
}

int ShaderProgram::CreateShader(const std::string& src, int type) {
	int id = glCreateShader(type);
	int vertlen = src.length();
	const char* vertxt = src.c_str();
	glShaderSource(id, 1, &vertxt, &vertlen);
	glCompileShader(id);
	if( !IsShaderCompilationSuccessful(id) ) {
		cerr << "Error while compiling shader" << endl;
		cerr << GetShaderInfoLog(id) << endl;
		throw ShaderException();
	}
	return id;
}

string ShaderProgram::GetShaderInfoLog(GLuint obj)
{
	int infologLength = 0;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	// ignore ending 0
	infologLength--;

	if (infologLength > 0) {
		int charsWritten  = 0;
		char* infoLog = new char[infologLength];
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		string strLog(infoLog);
		delete[] infoLog;
		return strLog;
	} else {
		return string("");
	}
}

string ShaderProgram::GetProgramInfoLog(GLuint obj)
{
	int infologLength = 0;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	// ignore ending 0
	infologLength--;

	if (infologLength > 0) {
		int charsWritten  = 0;
		char* infoLog = new char[infologLength];
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		delete[] infoLog;
		return string(infoLog);
	} else {
		return string("");
	}
}

bool ShaderProgram::IsShaderCompilationSuccessful(unsigned int obj)
{
	int status;
	glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
	return status == GL_TRUE;
}

bool ShaderProgram::IsProgramLinkingSuccessful(unsigned int obj)
{
	int status;
	glGetProgramiv(obj, GL_LINK_STATUS, &status);
	return status == GL_TRUE;

}

void ShaderProgram::printFragmentShaderInfoLog()
{
	string log = GetShaderInfoLog(my_fragment_shader);
	if( log.length() > 0 ) {
		std::cout << log << std::endl;
		std::cout << "Fragment shader: " << std::endl;
		std::cout << _info->fragment_src;
		std::cout << std::endl;
	}
}

void ShaderProgram::printVertexShaderInfoLog()
{
	string log = GetShaderInfoLog(my_vertex_shader);
	if( log.length() > 0 ) {
		std::cout << log << std::endl;
		std::cout << "Vertex shader: " << std::endl;
		std::cout << _info->vertex_src << std::endl;
		std::cout << std::endl;
	}
}

void ShaderProgram::printProgramInfoLog()
{
	string log = GetProgramInfoLog(my_program);
	if( log.length() > 0 ) {
		// write program to console
		std::cout << log << std::endl;
		std::cout << "--- START Geometry Shader ---" << std::endl;
		std::cout << _info->geometry_src;
		std::cout << "--- END Geometry Shader ---" << std::endl;
		std::cout << std::endl;
		std::cout << "--- START Vertex Shader ---" << std::endl;
		std::cout << _info->vertex_src;
		std::cout << "--- END Vertex Shader ---" << std::endl;
		std::cout << std::endl;
		std::cout << "--- START Fragment Shader ---" << std::endl;
		std::cout << _info->fragment_src;
		std::cout << "--- END Fragment Shader ---" << std::endl;
	}
}

void ShaderProgram::Use()
{
	AssureGenerated(); // lazy generation
	glUseProgram(my_program); // bind shader
}

}
