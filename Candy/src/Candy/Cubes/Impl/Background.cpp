#include "Background.h"
#include "../Cubes.h"
#include "../Rendering/CubesRenderling.h"
#include "DirectLighting.h"
#include "MCRT.h"
#include "RadiosityShooting.hpp"
#include "GroundHeightLookup.h"
#include "ExecuteInThreads.hpp"
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
	build_first_ = false;
	_running = false;
	gi_basic_.reset(new Hexa::Lighting::DirectLighting());
	gi_.reset(new Hexa::Lighting::MCRT());
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

struct Benchmarker
{
	Benchmarker()
	: count_(0), time_(0.0) {}
	void start() {
		timer.start();
	}
	void reset() {
		count_ = 0;
		time_ = 0.0;
	}
	void stop(size_t n) {
		timer.stop();
		count_ += n;
		time_ += timer.getElapsedTimeInSec();
	}
	size_t count() const {
		return count_;
	}
	double time() const {
		return time_;
	}
	double time_ms() const {
		return time_ * 1000.0;
	}
	double mean() const {
		return time_ / double(count_);
	}
	double mean_ms() const {
		return mean() * 1000.0;
	}
	double items_per_second() const {
		return double(count_) / time_;
	}
private:
	Danvil::Timer timer;
	size_t count_;
	double time_;
	size_t total_count_;
	double total_time_;
};

void Background::Run()
{
	const unsigned int cMaxCount = 4;
	const unsigned int cMaxThreads = 1;

	boost::timer print_timer;

#ifdef ENABLE_QUIT_AFTER_MAXTIME
	Danvil::Timer timer_all;
	timer_all.start();
#endif

	Benchmarker b_created, b_vitalized, b_mesh, b_height, b_lighting, b_lighting_gi;
	Benchmarker b_total_created, b_total_vitalized, b_total_mesh, b_total_height, b_total_lighting, b_total_lighting_gi;

	bool lighting_started_ = false;
	bool needs_restart = true;
	int restarted_cnt = 0;

	// the monster background loop
	while(_running)
	{
		// create cells
		size_t n_created = 0;
		{
			b_created.start();
			b_total_created.start();
			// pick cells which need recreation
			std::vector<Cell*> cells = cubes_->GetCellsIf([](Cell* cell) { return cell->NeedsCreation(); });
			// FIXME sort by visibility
			// create
			n_created = CandyCubes::ExecuteInThreads(cells, cMaxCount, cMaxThreads, [&](Cell* cell) {
						cubes_->CreateCell(cell, generator_.get());
			});
			b_created.stop(n_created);
			b_total_created.stop(n_created);
		}

		// vitalize dirty cells
		size_t n_vitalized = 0;
		{
			b_vitalized.start();
			b_total_vitalized.start();
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
			n_vitalized = CandyCubes::ExecuteInThreads(cells, cMaxCount, cMaxThreads, [&](Cell* cell) {
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
						neigbour_cell->ResetLighting();// = std::max(0u, (unsigned int)(samples_p * (float)(neigbour_cell->_lighting_samples)));
					});
			});
			b_vitalized.stop(n_vitalized);
			b_total_vitalized.stop(n_vitalized);
		}

		// lighting
		{
			if(gi_basic_) {
				b_lighting.start();
				b_total_lighting.start();
				size_t n_lighting = gi_basic_->Iterate(cubes_);
				b_lighting.stop(n_lighting);
				b_total_lighting.stop(n_lighting);
			}
			if(gi_) {
				if(lighting_started_ && (n_vitalized > 0)) {
					needs_restart = true;
				}
				if(!build_first_ || (n_created == 0 && n_vitalized == 0)) {
					if(needs_restart) {
						gi_->Restart();
						for(CellIterator cit=cubes_->IterateCells(); cit; ++cit) {
							for(Cell::BorderSideIterator sit=cit->IterateBorderSides(); sit; ++sit) {
								sit.data()->mark();
							}
						}
						restarted_cnt ++;
						needs_restart = false;
						std::cout << "Restarted lighting" << std::endl;
					}
					else {
						b_lighting_gi.start();
						b_total_lighting_gi.start();
						size_t n_lighting_gi = gi_->Iterate(cubes_);
						b_lighting_gi.stop(n_lighting_gi);
						b_total_lighting_gi.stop(n_lighting_gi);
						lighting_started_ = true;
					}
				}
			}
		}

		// recompute ground test
		{
			b_height.start();
			b_total_height.start();
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
			size_t n_height = cell_coordinates.size();
			b_height.stop(n_height);
			b_total_height.stop(n_height);
		}

		// recreate mesh data if necessary
		{
			b_mesh.start();
			b_total_mesh.start();
			size_t n_mesh = osgman_->UpdateMeshAll();
			b_mesh.stop(n_mesh);
			b_total_mesh.stop(n_mesh);
		}

		// print status message
		if(print_timer.elapsed() > 5.0) {
			print_timer.restart();
			std::cout << std::setprecision(3);
			std::cout
					<< "Created: " << b_created.count() << " (" << b_created.time() << " s)\t"
					<< "Vitalized: " << b_vitalized.count() << " (" << b_vitalized.time() << " s)\t"
					<< "Mesh: " << b_mesh.count() << " (" << b_mesh.time() << " s)\t"
					<< "Height: " << b_height.count() << " (" << b_height.time() << " s)\t"
					<< "Lighting: " << b_lighting.count() << " (" << b_lighting.items_per_second() << " samples/s)\t"
					<< "GI: " << b_lighting_gi.count() << " (" << b_lighting_gi.items_per_second() << " samples/s)" << std::endl;
			b_created.reset();
			b_vitalized.reset();
			b_mesh.reset();
			b_height.reset();
			b_lighting.reset();
			b_lighting_gi.reset();

			// FIXME
			std::cout << "Summarized Benchmark: " << std::endl;
			std::cout << std::setprecision(3);
			std::cout
					<< "Created   count: " << b_total_created.count() << ",\t time: " << b_total_created.time() << " s,\t mean: " << b_total_created.mean_ms() << " ms/cell" << std::endl
					<< "Vitalized count: " << b_total_vitalized.count() << ",\t time: " << b_total_vitalized.time() << " s,\t mean: " << b_total_vitalized.mean_ms() << " ms/cell" << std::endl
					<< "Mesh      count: " << b_total_mesh.count() << ",\t time: " << b_total_mesh.time() << " s,\t mean: " << b_total_mesh.mean_ms() << " ms/cell" << std::endl
					<< "Height    count: " << b_total_height.count() << ",\t time: " << b_total_height.time() << " s,\t mean: " << b_total_height.mean_ms() << " ms/cell" << std::endl
					<< "Lighting  count: " << b_total_lighting.count() << ",\t time: " << b_total_lighting.time() << " s,\t speed: " << b_total_lighting.items_per_second() << " samples/s" << std::endl
					<< "GI        count: " << b_total_lighting_gi.count() << ",\t time: " << b_total_lighting_gi.time() << " s,\t speed: " << b_total_lighting_gi.items_per_second() << " samples/s" << std::endl;

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

	std::cout << "Summarized Benchmark: " << std::endl;
	std::cout << std::setprecision(3);
	std::cout
			<< "Created   count: " << b_total_created.count() << ",\t time: " << b_total_created.time() << " s,\t mean: " << b_total_created.mean_ms() << " ms/cell" << std::endl
			<< "Vitalized count: " << b_total_vitalized.count() << ",\t time: " << b_total_vitalized.time() << " s,\t mean: " << b_total_vitalized.mean_ms() << " ms/cell" << std::endl
			<< "Mesh      count: " << b_total_mesh.count() << ",\t time: " << b_total_mesh.time() << " s,\t mean: " << b_total_mesh.mean_ms() << " ms/cell" << std::endl
			<< "Height    count: " << b_total_height.count() << ",\t time: " << b_total_height.time() << " s,\t mean: " << b_total_height.mean_ms() << " ms/cell" << std::endl
			<< "Lighting  count: " << b_total_lighting.count() << ",\t time: " << b_total_lighting.time() << " s,\t speed: " << b_total_lighting.items_per_second() << " samples/s" << std::endl
			<< "GI        count: " << b_total_lighting_gi.count() << ",\t time: " << b_total_lighting_gi.time() << " s,\t speed: " << b_total_lighting_gi.items_per_second() << " samples/s" << std::endl;

}
