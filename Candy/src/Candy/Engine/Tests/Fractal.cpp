#include "Fractal.h"
#include "Crate.h"
#include "../Resources/MeshFactory.h"

namespace Candy
{
	Ptr(Candy::Node) Fractal::ConstructCrateArray()
	{
		Ptr(Candy::Node) root(new Candy::Node());
		for(int x=-2; x<=+2; x++) {
			for(int y=-2; y<=+2; y++) {
				for(int z=-2; z<=+2; z++) {
					Ptr(Candy::Node) node(new Candy::Node());
					node->SetVisual(new Candy::Crate(1.0f));
					Eigen::Vector3f p; p << (float)(3*x), (float)(3*y), (float)(3*z);
					node->SetPosition(p);
					root->Add(node);
				}
			}
		}
		return root;
	}

	Ptr(Candy::Node) ConstructSierpinskiImpl(float px, float py, float pz, float a, unsigned int depth)
	{
		a *= 0.5f;

		float h = std::sqrt(3.0f) / 2.0f * a;
		float dx = 0.5f * a;
		float dy1 = 0.33333333f * h;
		float dy2 = 0.66666667f * h;
		float dz = std::sqrt(6.0f) / 3.0f * a;

		Ptr(Candy::Node) root(new Candy::Node());
		if(depth == 0) {
			root->SetVisual(new Candy::Crate(MeshFactory::CreateTetraederPTN(2*h)));
			Eigen::Vector3f p; p << px, py, pz;
			root->SetPosition(p);
		} else {
			root->Add(ConstructSierpinskiImpl(
				px - dx, py - dy1, pz,
				a, depth-1));
			root->Add(ConstructSierpinskiImpl(
				px + dx, py - dy1, pz,
				a, depth-1));
			root->Add(ConstructSierpinskiImpl(
				px, py + dy2, pz,
				a, depth-1));
			root->Add(ConstructSierpinskiImpl(
				px, py, pz + dz,
				a, depth-1));
		}
		return root;
	}

	Ptr(Candy::Node) Fractal::ConstructSierpinski(unsigned int depth)
	{
		return ConstructSierpinskiImpl(0, 0, 0, std::pow(2,float(depth)), depth);
	}

	Ptr(Candy::Node) ConstructSierpinskiCubesImpl(float px, float py, float pz, float scale, unsigned int depth)
	{
		float a = 0.25f * scale;

		Ptr(Candy::Node) root(new Candy::Node());
		if(depth == 0) {
			root->SetVisual(new Candy::Crate(1.0f));
			Eigen::Vector3f p; p << px, py, pz;
			root->SetPosition(p);
		} else {
			root->Add(ConstructSierpinskiCubesImpl(
				px - a, py - a, pz - a,
				scale * 0.5f, depth-1));
			root->Add(ConstructSierpinskiCubesImpl(
				px, py + a, pz - a,
				scale * 0.5f, depth-1));
			root->Add(ConstructSierpinskiCubesImpl(
				px + a, py - a, pz - a,
				scale * 0.5f, depth-1));
			root->Add(ConstructSierpinskiCubesImpl(
				px, py, pz + a,
				scale * 0.5f, depth-1));
		}
		return root;
	}

	Ptr(Candy::Node) Fractal::ConstructSierpinskiCubes(unsigned int depth)
	{
		return ConstructSierpinskiCubesImpl(0, 0, 0, std::pow(2.0f,float(depth)), depth);
	}

}
