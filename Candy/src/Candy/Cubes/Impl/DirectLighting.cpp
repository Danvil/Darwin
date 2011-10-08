#include "DirectLighting.h"
#include "LightingHelpers.hpp"
#include "../Appearance.h"
#include "../Cubes.h"
#include <cassert>

namespace Hexa
{
namespace Lighting
{
	float ComputeAmbientLight(Cubes* cubes, const CoordI& cw, int sideIndex)
	{
		return 0.35f;
		// FIXME estimate ambient illumination
	}

	/** Casts a light sample from a point and checks for collisions
	 * @param cubes
	 * @param a sample start point
	 * @param u sample direction
	 * @param n surface normal at start point
	 * @return light percentage at point
	 */
	float ComputeSampleLight(Cubes* cubes, const Vec3f& a, const Vec3f& u, const Vec3f& n)
	{
		// intersect ray with other cubes
		Ci c;
		float distance;
		int side;
		bool hits = cubes->Pick(a, u, c, distance, side); // TODO we do not need the distance!
		if(!hits) {
			return n.dot(u);
		}
		else {
			return 0.0f;
		}
	}

	float ComputeSunLightSimple(Cubes* cubes, const CoordI& cw, int sideIndex) {
		Vec3f n = Properties::SideNormal(cw, sideIndex);
		return n.dot(Appearance::SunPosition);
	}

	float ComputeSunLightRandomSamples(Cubes* cubes, const CoordI& cw, int sideIndex)
	{
		const unsigned int cSamples = 5;
		const float cCenterWeight = 0.4f;
		Vec3f pos = Properties::WorldToPositionCenter(cw);
		Vec3f n = Properties::SideNormal(cw, sideIndex);
//		// center
//		float w_center = ComputeSampleLight(cubes, pos, Appearance::SunPosition, n);
//		if(cSamples == 1) {
//			return w_center;
//		}
		// create samples from a random side position towards the sun
		float w_soft = 0.0f;
		// this creates soft shadows
		for(unsigned int s=1; s<cSamples; s++) {
			// cast ray from random position on cube side
			Vec3f a = pos + RandomCubeSidePoint(cw, sideIndex);
			w_soft += ComputeSampleLight(cubes, a, Appearance::SunPosition, n);
		}
//		return cCenterWeight * w_center + (1.0f - cCenterWeight) * w_soft / float(cSamples - 1);
		return w_soft / float(cSamples);
	}

	float ComputeSunLightPatternSamples(Cubes* cubes, const CoordI& cw, int sideIndex)
	{
		Vec3f pos = Properties::WorldToPositionCenter(cw);
		Vec3f n = Properties::SideNormal(cw, sideIndex);
		// create samples from cube side towards the sun
		float sum = 0.0f;
		// this creates soft shadows
		for(unsigned int s=0; s<PatternCubeSidePointCount; s++) {
			// cast ray from random position on cube side
			Vec3f a = pos + PatternCubeSidePoint(cw, sideIndex, s);
			sum += ComputeSampleLight(cubes, a, Appearance::SunPosition, n);
		}
		return sum / float(PatternCubeSidePointCount);
	}

	const unsigned int cThreadCount = 1;

//	struct CellByEyeDistance {
//		bool operator()(Cell* a, Cell* b) {
//			// lighting is not computed for cells which needs vitalization
//			if(a->NeedsVitalization()) {
//				return false; // take b
//			}
//			if(b->NeedsVitalization()) {
//				return true; // take a
//			}
//			// omit cells with no border
//			if(!a->HasBorder()) {
//				return false;
//			}
//			if(!b->HasBorder()) {
//				return true;
//			}
//			// decide by distance
//			float da = Core::DistanceToEye(Properties::CellMid(a->coordinate()));
//			float db = Core::DistanceToEye(Properties::CellMid(b->coordinate()));
//			return da < db;
//		}
//	};

	uint64_t DirectLighting::Iterate(Ptr(Cubes) cubes)
	{
		uint64_t this_count = 0;

		// pick cells with minimal samples and sort
		std::vector<Cell*> cells = cubes->GetCellsIf([](Cell* cell) {
			return cell->CountLightingSamples() == 0 && cell->HasBorder() && !cell->IsContentChanged();
		});
//		std::cout << "Cells which need lighting " << cells.size() << std::endl;
//		std::sort(cells.begin(), cells.end(), CellByEyeDistance());

		if(cells.size() > 0) {

			size_t thread_count = std::min((size_t)cThreadCount, cells.size());

			if(thread_count == 1) {
				size_t i = Random::Uniform(cells.size() - 1);
				Cell* cell = cells[i];
				ComputeCell(cubes.get(), cell);
				this_count += cell->BorderSideCount();
			} else {
				std::vector<boost::thread*> threads;
				for(size_t i=0; i<thread_count; i++) {
					threads.push_back(new boost::thread(&ComputeCell, cubes.get(), cells[i]));
					this_count += cells[i]->BorderSideCount() * 1;
				}
				for(size_t i=0; i<thread_count; i++) {
					threads[i]->join();
					delete threads[i];
				}
			}
		}
		return this_count;
	}

	void ComputeCubeImpl(Cubes* cubes, const CoordI& cw, unsigned int sideIndex, CubeType type, CubeSideData* data)
	{
		data->lighting.ambient = ComputeAmbientLight(cubes, cw, sideIndex);
		data->lighting.sun = ComputeSunLightSimple(cubes, cw, sideIndex);
		data->lighting.scenery = Appearance::CubeEmitColor(type);
	}

	void DirectLighting::ComputeCell(Cubes* cubes, Cell* cell)
	{
		for(Cell::BorderSideIterator it=cell->IterateBorderSides(); it; ++it) {
			ComputeCubeImpl(cubes, it.world(), it.side(), it.type(), it.data());
		}
		cell->AddSamples(1);
		cell->SetAppearanceDirtyFlag();
	}

	void DirectLighting::ComputeCube(Cubes* cubes, const Cell::BorderSideIterator& it)
	{
		ComputeCubeImpl(cubes, it.world(), it.side(), it.type(), it.data());
	}

}
}
