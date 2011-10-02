/*
 * Benchmark.h
 *
 *  Created on: 04.02.2010
 *  Changed on: 22.09.2010
 *      Author: david
 */
#ifndef DANVIL_TOOLS_BENCHMARK_H_
#define DANVIL_TOOLS_BENCHMARK_H_
//---------------------------------------------------------------------------
#include "Timer.h"
#include "Ptr.h"
#include <boost/thread.hpp>
#include <iostream>
#include <string>
#include <map>
#include <vector>
//---------------------------------------------------------------------------
namespace Danvil {
//---------------------------------------------------------------------------

class Benchmark
{
public:
	Benchmark()
	{}

	virtual ~Benchmark()
	{}

	struct Data {
		Data()
		: useDetail(false),
		  n(0),
		  sum(0.0f)
		{}

		bool useDetail;

		size_t n;

		double sum;

		std::vector<double> detailed;

		void add(double t, size_t w=1) {
			if(useDetail) {
				detailed.push_back(t);
				throw 0; // FIXME implement
			} else {
				n += w;
				sum += t;
			}
		}

		unsigned int count() const {
			if(useDetail) {
				return detailed.size();
			} else {
				return n;
			}
		}

		double total() const
		{
			if(useDetail) {
				return 0;//Danvil::memops::sum(detailed.begin().base(), detailed.size());
			} else {
				return sum;
			}
		}

		double mean() const
		{
			if(useDetail) {
				return 0;//Danvil::memops::mean(detailed.begin().base(), detailed.size());
			} else {
				return sum / (double)n;
			}
		}

		double sigma() const
		{
			if(useDetail) {
				return 0;//Danvil::memops::rootMeanSquareDeviation<double,double>(detailed.begin().base(), detailed.size(), mean());
			} else {
				return 0; // undefined
			}
		}
	};

	void add(const std::string& tag, double time, size_t weight=1) {
		boost::mutex::scoped_lock lock(mutex_);
		tag_data_[tag].add(time, weight);
	}

	void print(std::ostream& os, const std::string& tag) {
		boost::mutex::scoped_lock lock(mutex_);
		os << "Benchmark for '" << tag << "': ";
		Data& a = tag_data_[tag];
		os << "count=" << a.count() << ", mean=" << a.mean() << ", total=" << a.total();
		if(a.useDetail) {
			os << ", sigma=" << a.sigma();
		}
		os << std::endl;
	}

	void printAll(std::ostream& os) {
		for(TagContainer::iterator it=tag_data_.begin(); it!=tag_data_.end(); ++it) {
			print(os, it->first);
		}
	}

public:
	static Benchmark& Instance() {
		static Benchmark* benchmark = 0;
		if(benchmark == 0) {
			benchmark = new Benchmark();
		}
		return *benchmark;
	}

//	static Benchmark& Instance() {
//		static Benchmark benchmark;
//		return benchmark;
//	}

private:
	boost::mutex mutex_;

	typedef std::map<std::string, Data> TagContainer;

	TagContainer tag_data_;

};

//---------------------------------------------------------------------------

#ifdef DANVIL_ENABLE_BENCHMARK
	#define DANVIL_BENCHMARK_START(TOKEN) \
		Danvil::Timer timer##TOKEN; timer##TOKEN.start();
	#define DANVIL_BENCHMARK_STOP(TOKEN) \
		timer##TOKEN.stop(); \
		Danvil::Benchmark::Instance().add(#TOKEN, timer##TOKEN.getElapsedTimeInMilliSec());
	#define DANVIL_BENCHMARK_STOP_WITH_NAME(TOKEN,NAME) \
		timer##TOKEN.stop(); \
		Danvil::Benchmark::Instance().add(NAME, timer##TOKEN.getElapsedTimeInMilliSec());
#define DANVIL_BENCHMARK_STOP_WITH_NAME_WITH_WEIGHT(TOKEN,NAME,WEIGHT) \
	timer##TOKEN.stop(); \
	Danvil::Benchmark::Instance().add(NAME, timer##TOKEN.getElapsedTimeInMilliSec(), WEIGHT);
#else
	#define DANVIL_BENCHMARK_START(TOKEN)
	#define DANVIL_BENCHMARK_STOP(TOKEN)
	#define DANVIL_BENCHMARK_STOP_WITH_NAME(TOKEN,NAME)
#endif

#define DANVIL_BENCHMARK_PRINTALL_COUT\
		Danvil::Benchmark::Instance().printAll(std::cout);

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
