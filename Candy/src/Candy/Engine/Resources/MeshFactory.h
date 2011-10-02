#pragma once

#include "../Mesh.h"
#include <Candy/Tools/Ptr.h>
#include <string>

namespace Candy
{
	namespace MeshFactory
	{
		/** Loads a mesh from a Wavefront obj file */
		Ptr(PTN::Mesh) LoadObjPTN(const std::string& fn);

		/** Loads a mesh from a Wavefront obj file */
		Ptr(PN::Mesh) LoadObjPN(const std::string& fn);

		/** Loads a mesh from a Wavefront obj file */
		Ptr(P::Mesh) LoadObjP(const std::string& fn);

		/** Creates a cube mesh with positions */
		Ptr(P::Mesh) CreateBoxP(float size);

		/** Creates a cube mesh with positions, texture coordinates and normals */
		Ptr(PTN::Mesh) CreateBoxPTN(float size);

		/** Creates a tetraeder mesh with positions */
		Ptr(P::Mesh) CreateTetraederP(float size);

		/** Creates a tetraeder mesh with positions, texture coordinates and normals */
		Ptr(PTN::Mesh) CreateTetraederPTN(float size);

	}
}
