#include "Background.h"
#include "Cubes.h"
#include "Rendering/CubesRenderling.h"
#include "Impl/DirectLighting.h"
#include "Impl/MCRT.h"
#include "Impl/Radiosity.hpp"
#include <Candy/Tools/Timer.h>
#include <iostream>
#undef min
#undef max
#include <algorithm>
#include <fstream>
#include <boost/timer.hpp>

//#define ENABLE_QUIT_AFTER_MAXTIME
const unsigned int cMaxComputationTime = 120000;
//#define ENABLE_PRINT_MEASUREMENTS_FILE
#define ENABLE_PRINT_BENCHMARK
#define SLEEP_TIME 20


Background::Background(Ptr(Cubes) cubes, Ptr(CubesRenderling) osgman, Ptr(Generator) generator)
: cubes_(cubes), osgman_(osgman), generator_(generator)
{
	_running = false;
	gi_basic_.reset(new Hexa::Lighting::DirectLighting());
//	gi_.reset(new Hexa::Lighting::MCRT());
//	gi_.reset(new Hexa::Lighting::Radiosity());
//	gi_.reset(new Hexa::Lighting::RadiosityShooting());
}


Background::~Background()
{
}

void Background::Start()
{
	if(_running) {
		return;
	}
	_running = true;
	_thread = boost::thread(&Background::Run, this);
}

void Background::Stop()
{
	if(!_running) {
		return;
	}
	_running = false;
	_thread.join();
}

void Background::PrintStatus()
{
	std::cout << "Non-empty cells=" << cubes_->CountNonEmptyCells() << " / ";
	std::cout << "Border quads=" << cubes_->CountBorderQuads() << " / ";
	std::cout << "RAM=" << cubes_->MemoryUsageMB() << " MB" << std::endl;

	std::vector<Cell*> cells = cubes_->GetCells();
	if(cells.size() == 0) {
		std::cout << "Cell status: No cells" << std::endl;
		return;
	}
	unsigned int num_cells = 0;
	unsigned int num_samples = 0;
	unsigned int num_samples_min = cells[0]->CountLightingSamples();
	unsigned int num_samples_max = cells[0]->CountLightingSamples();
	for(std::vector<Cell*>::const_iterator it=cells.begin(); it!=cells.end(); it++) {
		num_cells++;
		unsigned int n = (*it)->CountLightingSamples();
		num_samples += n;
		num_samples_min = std::min(num_samples_min, n);
		num_samples_max = std::max(num_samples_max, n);
	}
	double num_samples_mean = double(num_samples) / double(num_cells);
	std::cout << "Lighting samples: Min=" << num_samples_min << " / Mean=" << num_samples_mean << " / Max=" << num_samples_max << std::endl;
}

template<typename Op>
size_t Execute(const std::vector<Cell*>& cells, size_t max_count, bool use_threading, Op op)
{
	size_t count = std::min(max_count, cells.size());
	if(count == 0) {
		return 0;
	}
	if(use_threading && count > 1) {
		std::vector<boost::thread*> threads(count);
		for(size_t i=0; i<count; i++) {
			Cell* cell = cells[i];
			threads[i] = new boost::thread([&]() { op(cell); });
		}
		for(size_t i=0; i<count; i++) {
			threads[i]->join();
			delete threads[i];
		}
	}
	else {
		for(size_t i=0; i<count; i++) {
			op(cells[i]);
		}
	}
	return count;
}

void Background::Run()
{
	const unsigned int cMaxCount = 12;
	const bool cUseThreading = true;

	double TotalTime = 0.0;
	double TotalCount = 0.0;
	Danvil::Timer timer;

	boost::timer print_timer;

#ifdef ENABLE_QUIT_AFTER_MAXTIME
	Danvil::Timer timer_all;
	timer_all.start();
#endif

#ifdef ENABLE_PRINT_MEASUREMENTS_FILE
	std::ofstream fs("measurements.txt");
#endif

	// the monster background loop
	while(_running)
	{
		// recompute ground test
		//world_->RebuildGroundTest();

		// create cells
		{
			// pick cells which need recreation
			std::vector<Cell*> cells = cubes_->GetCellsIf([](Cell* cell) { return cell->NeedsCreation(); });
			// FIXME sort by visibility
			// create
			size_t n = Execute(cells, cMaxCount, cUseThreading, [&](Cell* cell) {
						cubes_->CreateCell(cell, generator_.get());
			});
			if(n > 0) {
				std::cout << "Created " << n << " cells" << std::endl;
			}
		}

		// vitalize dirty cells
		{
			// pick cells which need vitalization
			std::vector<Cell*> cells = cubes_->GetCellsIf([](Cell* cell) { return cell->NeedsVitalization(); });
			// FIXME sort by visibility
			// vitalize
			size_t n = Execute(cells, cMaxCount, cUseThreading, [&](Cell* cell) {
					cubes_->VitalizeCubeData(cell);
					// we also need to reset lighting for neighbouring cells!
					const int R = 2;
					for(int dz=-R; dz<=+R; dz++) {
						for(int dy=-R; dy<=+R; dy++) {
							for(int dx=-R; dx<=+R; dx++) {
								// removed samples vary from distance
								float d = std::sqrt(float(dx * dx + dy * dy + dz * dz));
								float samples_p = 0.0f + d * 0.25f;
								Cell* neigbour_cell;
								if(cubes_->TryGetCell(cell->coordinate() + Ci(dx, dy, dz), &neigbour_cell)) {
									neigbour_cell->_lighting_samples = std::max(0u, (unsigned int)(samples_p * (float)(neigbour_cell->_lighting_samples)));
								}
							}
						}
					}
			});
			if(n > 0) {
				std::cout << "Vitalized " << n << " cells" << std::endl;
			}
		}

		timer.start();
		uint64_t this_count = 0;
		if(gi_basic_) {
			this_count += gi_basic_->Iterate(cubes_);
		}
		if(gi_) {
			this_count += gi_->Iterate(cubes_);
		}
		timer.stop();
		double this_time = timer.getElapsedTimeInMilliSec();

#ifdef ENABLE_PRINT_MEASUREMENTS_FILE
			fs << this_count << "\t" << this_time << std::endl;
#endif
		TotalTime += this_time;
		TotalCount += double(this_count);

		{
			// recreate mesh data if necessary
			osgman_->UpdateMeshAll();
		}

		// print status message
#ifdef ENABLE_PRINT_BENCHMARK
		if(print_timer.elapsed() > 5.0) {
			print_timer.restart();
			PrintStatus();
			double sps = TotalCount / TotalTime * 1000.0;
			std::cout << "Lighting computation performance: " << int(sps) << " samples/s; " << "count=" << TotalCount << "; time=" << TotalTime << std::endl;
			//TotalTime = 0;
			//TotalCount = 0;
		}
#endif

#ifdef ENABLE_QUIT_AFTER_MAXTIME
		double elapsed = timer_all.getElapsedTimeInMilliSec();
		if(elapsed > cMaxComputationTime) {
			std::cout << "!!! Background thread quited after " << elapsed << " ms !!!" << std::endl;
			break;
		}
#endif

		boost::this_thread::sleep(boost::posix_time::milliseconds(SLEEP_TIME));
	}

#ifdef ENABLE_PRINT_MEASUREMENTS_FILE
	fs.close();
#endif

}
