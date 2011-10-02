#include "MCRT.h"
#include "LightingHelpers.hpp"
#include "../Appearance.h"
#include "../Cubes.h"
#include <cassert>

namespace Hexa
{
namespace Lighting
{
	const float cReflectionFactor = 0.95f / 3.1415f; // albedo 0.95

	inline void AddRayLightingAmbient(const Ptr(Cubes)& cubes, const Vec3f& pos, int sideIndex, CubeSideLightData& light) {
		// emit random ray from side
		Vec3f a = pos + RandomCubeSidePoint(sideIndex);
		float n_dot_u;
		Vec3f u = RandomDirectionFromCubeSide(sideIndex, n_dot_u);
		// intersect ray with other cubes
		Ci c;
		float distance;
		int side;
		bool hits = cubes->Pick(a, u, c, distance, side); // TODO we do not need the distance!
		// compute shadow
		if(hits) {
			CubeSideData* data = cubes->GetData(c, side);
			//assert(data); // TODO why is this happening? ignore for now ...
			if(data != 0) {
				float bdrf = n_dot_u * cReflectionFactor;
				light.ambient += bdrf * data->lighting.ambient;
				light.sun += bdrf * data->lighting.sun;
				light.scenery += bdrf * data->lighting.scenery_with_object;
			}
		} else {
			// uniform ambient background light with slight highlight towards the sun
			float q = u.dot(Appearance::SunPosition) / u.norm();
			float u = (q > 0.8f ? std::pow(q, 20) : 0.0f);
			light.ambient += 0.9f + 0.1f * u;
		}
	}

	inline void AddRayLightingSun(const Ptr(Cubes)& cubes, const Vec3f& pos, int sideIndex, CubeSideLightData& light) {
		// emit random ray from side towards sun
		Vec3f a = pos + RandomCubeSidePoint(sideIndex);
		Vec3f u = Appearance::SunPosition;//(1000.0f * Appearance::SunPosition - a).normalized();
		// intersect ray with other cubes
		Ci c;
		float distance;
		int side;
		bool hits = cubes->Pick(a, u, c, distance, side); // TODO we do not need the distance!
		// if cube sees the sun add 1
		if(!hits) {
			light.sun += 1.0f;
		}
	}

	void ComputeLighting(const Ptr(Cubes)& cubes, Cell* cell, unsigned int new_samples) {
		if(!cell->HasBorder()) {
			return;
		}
		unsigned int old_samples = 32 + cell->CountLightingSamples(); // FIXME HACK!
		float scl = 1.0f / float(old_samples + new_samples);
		cell->AddSamples(new_samples);
		for(Cell::BorderSideIterator it=cell->IterateBorderSides(); it; ++it) {
			// some data
			//const Ci& cc_world = it.world();
			CubeType type = it.type();
			Vec3f pos = it.positionCenter();
			CubeSideData* data = it.data();
			unsigned int sideIndex = it.side();
			const Vec3f& emit_color = Appearance::CubeEmitColor(type);
			// accumulate lighting
			CubeSideLightData lighting;
			for(unsigned int i=0; i<new_samples; i++) {
				AddRayLightingAmbient(cubes, pos, sideIndex, lighting);
				AddRayLightingSun(cubes, pos, sideIndex, lighting);
			}
			// update samples
			CubeSideLightData& target = data->lighting;
			target.ambient = scl * (float(old_samples) * target.ambient + lighting.ambient);
			target.sun = scl * (float(old_samples) * target.sun + lighting.sun);
			target.scenery = scl * (float(old_samples) * target.scenery + lighting.scenery + float(new_samples) * emit_color);
			target.scenery_with_object = data->object_color.cwiseProduct(target.scenery);
		}
	}

	const unsigned int cSamplesMin = 8;
	const unsigned int cSamplesMax = 1024;
	const float cSamplesPerDistance = 4.0f;
	const unsigned int cSamplesPerRoundMin = 16;
	const unsigned int cSamplesPerRoundMax = 128;
	const bool cUseMoreThanMax = true;
	#define ENABLE_UNIFORM_ORDER
	const unsigned int cThreadCount = 1;

	struct CellBySampleCount {
		bool operator()(Cell* a, Cell* b) {
	#ifdef ENABLE_UNIFORM_ORDER
			return a->CountLightingSamples() < b->CountLightingSamples();
	#else
			// decide by sample count and distance
			unsigned int na = a->CountLightingSamples();
			unsigned int nb = b->CountLightingSamples();
			float da = Core::DistanceToEye(Properties::CellMid(a->coordinate()));
			float db = Core::DistanceToEye(Properties::CellMid(b->coordinate()));
			// if both cells have less than minimal samples decide by distance
			if((na < cSamplesMin) && (nb < cSamplesMin)) {
				return da < db;
			}
			// if exactly one cell has less than minimal decide by count
			else if((na < cSamplesMin) || (nb < cSamplesMin)) {
				return na < nb;
			}
			// if both cells have more than maximal samples decide by distance
			if((na >= cSamplesMax) && (nb >= cSamplesMax)) {
				return da < db;
			}
			// if exactly one cell has more than maximal decide by count
			else if((na >= cSamplesMax) || (nb >= cSamplesMax)) {
				return na < nb;
			}
			// decide by sample count and distance to eye
			unsigned int da_w = unsigned int(da * cSamplesPerDistance);
			unsigned int db_w = unsigned int(db * cSamplesPerDistance);
			return na + da_w < nb + db_w;
	#endif
		}
	};

	void UpdateLighting(Ptr(Cubes) cubes, Cell* cell, unsigned int sample_count)
	{
		// compute light samples
		ComputeLighting(cubes, cell, sample_count);
		// notify that cell data has changed
		cubes->OnChangeCell(cell, false);
		// log
	//	std::cout << "Lighting for " << cell->coordinate() << " has " << cell->CountLightingSamples() << " added " << sample_count << std::endl;
	}

	uint64_t MCRT::Iterate(Ptr(Cubes) cubes_)
	{
		uint64_t this_count = 0;

		// pick cells with minimal samples and sort
		std::vector<Cell*> all_cells = cubes_->GetCells();
		std::vector<Cell*> cells;
		cells.reserve(all_cells.size());
		std::for_each(all_cells.begin(), all_cells.end(), [&](Cell* cell) {
			if(cell->CountLightingSamples() > 0 // FIXME HACK!
					&& cell->HasBorder()
					&& !cell->NeedsVitalization()
			) {
				cells.push_back(cell);
			}
		});
		std::sort(cells.begin(), cells.end(), CellBySampleCount());

		if(cells.size() > 0 && (cUseMoreThanMax || cells[0]->CountLightingSamples() < cSamplesMax)) {

			size_t thread_count = std::min((size_t)cThreadCount, cells.size());

			if(thread_count == 1) {
				// update lighting for element with least samples
				Cell* cell = cells[0];
				// TODO use min_element instead of sort!
				unsigned int current_sample_count = cell->CountLightingSamples();
				if(cUseMoreThanMax || current_sample_count < cSamplesMax) {
					unsigned int inc_sample_count = Math::Clamp(current_sample_count, cSamplesPerRoundMin, cSamplesPerRoundMax);
					UpdateLighting(cubes_, cell, inc_sample_count);
					this_count += double(cell->BorderSideCount() * inc_sample_count);
				}
			} else {
				// FIXME sample count!
				std::vector<boost::thread*> threads;
				unsigned int sample_count = cSamplesMin;
				for(size_t i=0; i<thread_count; i++) {
					if(cells[i]->CountLightingSamples() > sample_count) {
						sample_count = cells[i]->CountLightingSamples();
					}
				}
				for(size_t i=0; i<thread_count; i++) {
					threads.push_back(new boost::thread(&UpdateLighting, cubes_, cells[i], sample_count));
					this_count += cells[i]->BorderSideCount() * sample_count;
				}
				for(size_t i=0; i<thread_count; i++) {
					threads[i]->join();
					delete threads[i];
				}
			}
		}
		return this_count;
	}


}
}
