#pragma once

#include "Coord.h"
#include "Coord2.h"
#include <map>

namespace Containers
{
	template<typename T, int D=32, bool IsValueType=false>
	class Array2
	{
	public:
		template<typename U>
		void SetZero(U* u) {
		}

		template<typename U>
		void SetZero(U** u) {
			*u = 0;
		}

		typedef T value;
		typedef value* iterator;
		typedef const value* const_iterator;
		Array2() {
			if(!IsValueType) {
				for(iterator i=begin(); i!=end(); i++) {
					SetZero(i);
				}
			}
		}
		size_t size() const {
			return D*D; // FIXME ok?
		}
		iterator begin() {
			return iterator(data_);
		}
		iterator end() {
			return begin() + D*D;
		}
		const_iterator begin() const {
			return const_iterator(data_);
		}
		const_iterator end() const {
			return begin() + D*D;
		}
		const_iterator find(int x, int y) const {
			const_iterator it = begin() + index(x, y);
			if(!has_value(it)) {
				return end();
			}
			else {
				return it;
			}
		}
		iterator find(int x, int y) {
			//return (iterator)(((const Array*)this)->find(c));
			iterator it = begin() + index(x, y);
			if(!has_value(it)) {
				return end();
			}
			else {
				return it;
			}
		}
		value& operator()(int x, int y) {
			return access(begin() + index(x, y));
		}
		const value& operator()(int x, int y) const {
			return access(begin() + index(x, y));
		}
		const T& access(const_iterator it) const {
			return *it;
		}
		T& access(iterator it) {
			return *it;
		}
		bool has_value(iterator it) const {
			return IsValueType ? true : (*it != 0);
		}
		bool has_value(const_iterator it) const {
			return IsValueType ? true : (*it != 0);
		}
		template<typename F>
		void ForEach(F& f) {
			for(iterator i=begin(); i!=end(); i++) {
				f(access(i));
			}
		}
	private:
		static size_t index(int x, int y) {
			assert(-D/2 <= x && x < D/2);
			assert(-D/2 <= y && y < D/2);
			return (x + D/2) + D*(y + D/2);
		}
	public:
		T data_[D*D];
	};

	template<typename T>
	class StdMap2
		: public std::map<Coord2I, T>
	{
	public:
		typedef typename std::map<Coord2I, T>::iterator iterator;
		typedef typename std::map<Coord2I, T>::const_iterator const_iterator;
		T& operator()(int x, int y) {
			return (*this)[Coord2I(x,y)];
		}
		const T& operator()(int x, int y) const {
			return (*this)[Coord2I(x,y)];
		}
		template<typename F>
		void ForEach(F f) {
			for(iterator it=this->begin(); it!=this->end(); ++it) {
				f(it->second);
			}
		}
	};

}
