/*
 * Grid.h
 *
 *  Created on: 21.09.2011
 *      Author: david
 */

#ifndef GRID_H_
#define GRID_H_

#include <boost/utility.hpp>
#include <boost/thread.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <algorithm>
#include <cassert>

namespace Candy {
namespace Cubes {
namespace Impl
{
	/** Iterates over a densely populated iterator
	 * Example:
	 *  - V = Cell*
	 *  - It = std::vector<V>::iterator
//	 *  - Test = [](It it) -> bool { return *it != 0; }
//	 *  - Access = [](It it) -> V { return *it; }
	 */
	template<typename V, typename It=V*>
	struct SparseIterator
	{
	public:
		SparseIterator(const It& begin, const It& end)
		: end_(end) {
			current_ = find_next(begin);
		}
		template<typename W, typename Jt>
		SparseIterator(const SparseIterator<W,Jt>& it) {
			current_ = it.current();
			end_ = it.end();
		}
		It current() const { return current_; }
		It end() const { return end_; }
//		bool valid() const { return current_ != end_; }
		V& get() const { return *current_; }
//		operator bool() { return valid(); }
		V& operator*() { return get(); }
		V& operator->() { return get(); }
		SparseIterator& operator++() {
			current_ = find_next(boost::next(current_));
			return *this;
		}
		SparseIterator operator+(size_t n) {
			return SparseIterator(current_ + n, end_);
		}
		bool operator==(const SparseIterator& it) {
			assert(end() == it.end());
			return current() == it.current();
		}
		bool operator!=(const SparseIterator& it) {
			assert(end() == it.end());
			return current() != it.current();
		}
	private:
		It find_next(It it) {
			while(*it && it != end_) ++it;
			return it;
		}
	private:
		It current_, end_;
	};

	template<typename T, unsigned int D>
	struct Array3
	{
//		typedef SparseIterator<T> iterator_valid;
//		typedef SparseIterator<const T> const_iterator_valid;
		typedef T* iterator;
		typedef const T* const_iterator;
		Array3() {
			assert(D % 2 == 0);
			for(iterator i=begin(); i!=end(); ++i) {
				*i = 0;
			}
		}
		size_t size() const {
			return D*D*D; // FIXME ok?
		}
		iterator begin() {
//			return iterator(data_, data_+size());
			return iterator(data_);
		}
		iterator end() {
//			return iterator(data_+size(), data_+size());
			return iterator(data_ + size());
		}
		const_iterator begin() const {
//			return const_iterator(data_, data_+size());
			return const_iterator(data_);
		}
		const_iterator end() const {
//			return const_iterator(data_+size(), data_+size());
			return const_iterator(data_ + size());
		}
		const_iterator find(const CoordI& c) const {
			if(!valid(c)) {
				return end();
			}
			const_iterator it = begin() + index(c);
			if(*it == 0) {
				return end();
			}
			else {
				return it;
			}
		}
		iterator find(const CoordI& c) {
			// re-use const version
			const_iterator cit = ((const Array3*)this)->find(c);
//			return iterator((T*)(cit.current()), (T*)(cit.end()));
			return iterator(cit);
		}
		T& operator[](const CoordI& c) {
			return *(begin() + index(c));
		}
		const T& operator[](const CoordI& c) const {
			return *(begin() + index(c));
		}
//		const T& access(const_iterator it) const {
//			return *it;
//		}
//		T& access(iterator it) {
//			return *it;
//		}
//		bool has_value(iterator it) const {
//			return *it != 0;
//		}
//		bool has_value(const_iterator it) const {
//			return *it != 0;
//		}
	private:
		static bool valid(const CoordI& c) {
			return (-int(D/2) <= c.x && c.x < int(D/2))
					&& (-int(D/2) <= c.y && c.y < int(D/2))
					&& (-int(D/2) <= c.z && c.z < int(D/2));
		}
		static size_t index(const CoordI& c) {
			assert(valid(c));
			return size_t(c.x + int(D/2)) + int(D)*(c.y + int(D/2)) + int(D*D)*(c.z + int(D/2));
		}
	public:
		T data_[D*D*D];
	};

	/*template<typename T>
	class StdMap3
		: public std::map<CoordI, T>
	{
	public:
		const T& access(const const_iterator& it) const {
			return it->second;
		}
		T& access(const iterator& it) {
			return it->second;
		}
		bool has_value(iterator it) const {
			return it->second != 0;
		}
		bool has_value(const_iterator it) const {
			return it->second != 0;
		}
	};*/
}}}

/** A container which holds an element of type T for each 3D location */
template<typename T>
struct CellContainerType {
	typedef Candy::Cubes::Impl::Array3<T,128> Result;
};

/**
 * T must be a pointer to something, i.e. operator bool() and =0 must be defined
 */
template<typename T>
struct Grid
{
public:
	typedef typename CellContainerType<T>::Result CellContainer;

	bool valid(const CoordI& c) const {
		boost::interprocess::scoped_lock<boost::mutex> guard(cells_mutex_);
		return cells_.find(c) != cells_.end();
	}

	void set(const CoordI& c, T v) {
		boost::interprocess::scoped_lock<boost::mutex> guard(cells_mutex_);
		cells_[c] = v;
	}

	/**
	 * @brief gets an element if it exists or null if not
	 * @param c cell coordinate of element
	 */
	T get(const CoordI& c) const {
		boost::interprocess::scoped_lock<boost::mutex> guard(cells_mutex_);
		typename CellContainer::const_iterator it = cells_.find(c);
		if(it == cells_.end()) {
			return 0;
		}
		else {
			return *it;
		}
	}

	/**
	 * @brief gets an element creating it if it does not yet exists
	 * @param c cell coordinate of element
	 * @param creator a functor () -> T used to create elements
	 */
	template<typename Op>
	T get(const CoordI& c, Op creator) {
		boost::interprocess::scoped_lock<boost::mutex> guard(cells_mutex_);
		typename CellContainer::const_iterator it = cells_.find(c);
		if(it == cells_.end()) {
			T& x = cells_[c];
			x = creator();
			return x;
		}
		else {
			return *it;
		}
	}

	T operator[](const CoordI& c) const {
		return get(c);
	}

	CellContainer cells() const {
		boost::interprocess::scoped_lock<boost::mutex> guard(cells_mutex_);
		return cells_;
	}

	std::vector<T> valid_cells() const {
		UpdateValid(); // FIXME
		boost::interprocess::scoped_lock<boost::mutex> guard(cells_mutex_);
		return cells_valid_;
	}

//	const std::vector<T>& active_cells() const {
//		return cells_active_;
//	}
//
	void UpdateValid() const {
		boost::interprocess::scoped_lock<boost::mutex> guard(cells_mutex_);
		cells_valid_.clear();
		cells_valid_.reserve(cells_.size());
		for(typename CellContainer::iterator it=cells_.begin(); it!=cells_.end(); ++it) {
			if(*it) {
				cells_valid_.push_back(*it);
			}
		}
	}

//	template<typename Op>
//	void UpdateActive(Op op) {
//		boost::interprocess::scoped_lock<boost::mutex> guard(cells_mutex_);
//		cells_active_.clear();
//		cells_active_.reserve(cells_.size());
//		for(typename CellContainer::iterator it=cells_.begin(); it!=cells_.end(); ++it) {
//			if(op(*it)) {
//				cells_active_.push_back(it);
//			}
//		}
//	}

private:
	mutable boost::mutex cells_mutex_;
	mutable CellContainer cells_;
	mutable std::vector<T> cells_valid_;
//	std::vector<T> cells_active_;
};

#endif
