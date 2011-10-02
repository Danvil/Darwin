#pragma once 

#include "Resource.h"
#include "ResourceInfo.h"
#include <Candy/Tools/Ptr.h>
#include <string>

namespace Candy {

	struct ShaderException {};

	class ShaderProgram
		: public Ressource
	{
	public:
		/** Creats a simple test shader. */
		ShaderProgram(const Ptr(ShaderInfo)& info);

		/** Generates OpenGl shader resources */
		void Generate();

		/** Frees resources */
		void Delete();

		void printFragmentShaderInfoLog();

		void printVertexShaderInfoLog();

		void printProgramInfoLog();

		/** Returns shader OpenGL ID. */
		int getProgram() const;

		/** Returns the location ID of a shader attribute */
		int getAttributeLocation(const std::string& attribute_name) const;

		/** Uses this shader for rendering. */
		void Use();

	private:
		static int CreateShader(const std::string& src, int type);
		static std::string GetShaderInfoLog(unsigned int obj);
		static std::string GetProgramInfoLog(unsigned int obj);
		static bool IsShaderCompilationSuccessful(unsigned int obj);
		static bool IsProgramLinkingSuccessful(unsigned int obj);

	private:
		Ptr(ShaderInfo) _info;

		int my_geometry_shader;
		int my_vertex_shader;
		int my_fragment_shader;
		int my_program;

	};

}
