#pragma once

#include <vector>
#include <algorithm>

namespace MoreStd
{

	template<typename T, typename F>
	void foreach(const T& q, F f) {
		std::for_each(q.begin(), q.end(), f);
	}

	template<typename T, typename V>
	void remove_first(T& v, const V& x) {
		typename T::iterator it = std::find(v.begin(), v.end(), x);
		if(it != v.end()) {
			v.erase(it);
		}
	}

	template<typename T, typename V>
	void remove_all(T& v, const V& x) {
		typename T::iterator it = std::find(v.begin(), v.end(), x);	
		while(it != v.end()) {
			v.erase(it);
			it = std::find(it, v.end(), x);	
		} 
	}

	template<typename T, typename K>
	size_t find_index(const T& v, K a) {
		typename T::const_iterator it = std::find(v.begin(), v.end(), a);
		if(it == v.end()) {
			return size_t(-1);
		}
		else  {
			return it - v.begin();
		}
	}

}
