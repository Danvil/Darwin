#include "Background.h"
#include "Cubes.h"
#include "Rendering/CubesRenderling.h"
#include "Impl/DirectLighting.h"
#include "Impl/MCRT.h"
#include "Impl/Radiosity.hpp"
#include "Impl/GroundHeightLookup.h"
#include "Impl/ExecuteInThreads.hpp"
#include <Candy/Tools/Timer.h>
#include <iostream>
#undef min
#undef max
#include <set>
#include <algorithm>
#include <fstream>
#include <boost/timer.hpp>

//#define ENABLE_QUIT_AFTER_MAXTIME
//const unsigned int cMaxComputationTime = 120000;
#define SLEEP_TIME 1


Background::Background(Ptr(Cubes) cubes, Ptr(CubesRenderling) osgman, Ptr(GroundHeightLookup) height, Ptr(Generator) generator)
: cubes_(cubes), osgman_(osgman), height_lookup_(height), generator_(generator)
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

void Background::Run()
{
	const unsigned int cMaxCount = 4;
	const unsigned int cMaxThreads = 1;

	Danvil::Timer timer;

	boost::timer print_timer;

#ifdef ENABLE_QUIT_AFTER_MAXTIME
	Danvil::Timer timer_all;
	timer_all.start();
#endif

	size_t n_created=0, n_vitalized=0, n_height=0, n_lighting=0;
	double time_created=0, time_vitalized=0, time_height=0, time_lighting=0;

	// the monster background loop
	while(_running)
	{
		// create cells
		timer.start();
		{
			// pick cells which need recreation
			std::vector<Cell*> cells = cubes_->GetCellsIf([](Cell* cell) { return cell->NeedsCreation(); });
			// FIXME sort by visibility
			// create
			n_created += CandyCubes::ExecuteInThreads(cells, cMaxCount, cMaxThreads, [&](Cell* cell) {
						cubes_->CreateCell(cell, generator_.get());
			});
		}
		timer.stop();
		time_created += timer.getElapsedTimeInMilliSec();

		// vitalize dirty cells
		timer.start();
		{
			// pick cells which need vitalization
			std::vector<Cell*> cells = cubes_->GetCellsIf([&](Cell* cell) {
				// only cells which need it
				if(!cell->IsContentChanged()) {
					return false;
				}
				// check that all face neighbours are created
				bool all_neighbours_created = true;
				cubes_->ApplyToFaceNeighboursCells(cell, [&](Cell* face_neighbour) {
					all_neighbours_created &= (face_neighbour == 0 || face_neighbour->IsCreated());
				});
				return all_neighbours_created;
			});
			// FIXME sort by visibility
			// vitalize
			n_vitalized += CandyCubes::ExecuteInThreads(cells, cMaxCount, cMaxThreads, [&](Cell* cell) {
					cubes_->VitalizeCubeData(cell);
					// we also need to reset lighting for neighbouring cells!
					cubes_->ApplyToNeighbourCells(cell, 2, [&](Cell* neigbour_cell) {
						if(neigbour_cell == 0) {
							return;
						}
						CoordI d = neigbour_cell->coordinate() - cell->coordinate();
						int dx = d.x;
						int dy = d.y;
						int dz = d.z;
						// removed samples vary from distance
						float distance = std::sqrt(float(dx * dx + dy * dy + dz * dz));
						float samples_p = 0.0f + distance * 0.25f;
						neigbour_cell->_lighting_samples = std::max(0u, (unsigned int)(samples_p * (float)(neigbour_cell->_lighting_samples)));
					});
			});
		}
		timer.stop();
		time_vitalized += timer.getElapsedTimeInMilliSec();

		// lighting
		timer.start();
		{
			if(gi_basic_) {
				n_lighting += gi_basic_->Iterate(cubes_);
			}
			if(gi_) {
				n_lighting += gi_->Iterate(cubes_);
			}
		}
		timer.stop();
		time_lighting += timer.getElapsedTimeInMilliSec();

		// recompute ground test
		timer.start();
		{
			// pick cells which need height update
			std::vector<Cell*> cells = cubes_->GetCellsIf([](Cell* cell) { return cell->IsHeightDirty(); });
			// get (unique) coordinates
			std::set<CoordI> cell_coordinates;
			std::for_each(cells.begin(), cells.end(), [&](Cell* cell) {
				cell_coordinates.insert(cell->coordinate());
			});
			std::vector<CoordI> cell_coordinates_v(cell_coordinates.begin(), cell_coordinates.end());
			CandyCubes::ExecuteInThreads(cell_coordinates_v, cMaxCount, cMaxThreads, [&](const CoordI& c_cell) {
				height_lookup_->Build(cubes_, c_cell);
			});
			std::for_each(cells.begin(), cells.end(), [&](Cell* cell) {
				cell->ClearHeightDirtyFlag();
			});
			n_height += cell_coordinates.size();
		}
		timer.stop();
		time_height += timer.getElapsedTimeInMilliSec();

		// recreate mesh data if necessary
		{
			osgman_->UpdateMeshAll();
		}

		// print status message
		if(print_timer.elapsed() > 3.0) {
			print_timer.restart();
//			PrintStatus();
//			double fps_created = double(n_created) / double(time_created);
//			double fps_vitalized = double(n_vitalized) / double(time_vitalized);
//			double fps_height = double(n_height) / double(time_height);
			double fps_lighting = double(n_lighting) / double(time_lighting);
			double mean_created = double(time_created) / double(n_created);
			double mean_vitalized = double(time_vitalized) / double(n_vitalized);
			double mean_height = double(time_height) / double(n_height);
//			double mean_lighting = double(time_lighting) / double(n_lighting);
			std::cout
					<< "Created: " << n_created << " (" << time_created << " ms)\t"
					<< "Vitalized: " << n_vitalized << " (" << time_vitalized << " ms)\t"
					<< "Height: " << n_height << " (" << time_height << " ms)\t"
					<< "Lighting: " << n_lighting << " (" << time_lighting << " ms - " << fps_lighting << " samples/ms)" << std::endl;
//					<< "Vitalized: " << n_vitalized << "\tHeight: " << n_height << std::endl;
//			std::cout << "Lighting computation performance: " << int(sps) << " samples/s; " << "count=" << n_lighting << "; time=" << time_lighting << std::endl;
			n_created = n_vitalized = n_height = n_lighting = 0;
			time_created = time_vitalized = time_height = time_lighting = 0;
		}

#ifdef ENABLE_QUIT_AFTER_MAXTIME
		double elapsed = timer_all.getElapsedTimeInMilliSec();
		if(elapsed > cMaxComputationTime) {
			std::cout << "!!! Background thread quited after " << elapsed << " ms !!!" << std::endl;
			break;
		}
#endif

		boost::this_thread::sleep(boost::posix_time::milliseconds(SLEEP_TIME));
	}

}
