/*
 * ExecuteInThreads.hpp
 *
 *  Created on: Oct 8, 2011
 *      Author: david
 */

#ifndef EXECUTEINTHREADS_HPP_
#define EXECUTEINTHREADS_HPP_

#include "../Cell.h"
#include <boost/thread.hpp>
#include <vector>

namespace CandyCubes
{
	template<typename T, typename Op>
	size_t ExecuteInThreads(const std::vector<T>& items, size_t max_count, size_t max_threads, Op op)
	{
		size_t count = std::min(max_count, items.size());
		if(count == 0) {
			return 0;
		}
		if(max_threads > 1 && count > 1) {
			int n = (int)boost::posix_time::time_duration::ticks_per_second() / 1000;
			boost::posix_time::time_duration delay(0,0,0,n);
			size_t current = 0;
			std::list<boost::thread*> threads;
			while(threads.size() > 0 || current < count) {
				// wait for threads
				std::list<boost::thread*> old_threads = threads;
				threads.clear(); // collect new threads here
				for(auto it=old_threads.begin(); it!=old_threads.end(); ++it) {
					if((*it)->timed_join(delay)) {
						// thread is ready
						delete *it;
					}
					else {
						// thread needs more time
						threads.push_back(*it);
					}
				}
				// create new threads
				while(threads.size() < max_threads && current < count) {
					T item = items[current];
					boost::thread* t = new boost::thread([&]() { op(item); });
					threads.push_back(t);
					current++;
				}
			}
//			size_t current = 0;
//			while(current < count) {
//				size_t n = std::min(max_threads, count - current);
//				std::vector<boost::thread*> threads;
//				for(size_t i=0; i<count; i++) {
//					T cell = items[i];
//					threads[i] = new boost::thread([&]() {
//						op(cell);
//					});
//				}
//				for(size_t i=0; i<count; i++) {
//					threads[i]->join();
//					delete threads[i];
//				}
//				current += n;
//			}
		}
		else {
			// single threading
			for(size_t i=0; i<count; i++) {
				op(items[i]);
			}
		}
		return count;
	}

}

#endif
