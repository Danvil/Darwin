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
#include <map>
#include <unordered_map>
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
		typedef T* iterator;
		typedef const T* const_iterator;
		Array3() {
			assert(D % 2 == 0);
			for(iterator i=begin(); i!=end(); ++i) {
				*i = 0;
			}
		}
		size_t size() const {
			return D*D*D;
		}
		iterator begin() {
			return iterator(data_);
		}
		iterator end() {
			return iterator(data_ + size());
		}
		const_iterator begin() const {
			return const_iterator(data_);
		}
		const_iterator end() const {
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
		void set(const CoordI& c, T v) {
			*(begin() + index(c)) = v;
		}
//		T& operator[](const CoordI& c) {
//			return *(begin() + index(c));
//		}
		const T& operator[](const CoordI& c) const {
			return *(begin() + index(c));
		}
		template<typename It, typename Jt>
		static bool Equals(const It& a, const Jt& b) {
			return a == b;
		}
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

	template<typename T>
	class StdMap3
	{
	public:
		template<typename It>
		struct MapValueIterator {
			MapValueIterator() {}
			template<typename Jt> MapValueIterator(const MapValueIterator<Jt>& a) : it_(a.get()) {}
			template<typename Jt> MapValueIterator(Jt it) : it_(it) {}
			template<typename Jt> MapValueIterator& operator=(const MapValueIterator<Jt>& a) { it_ = a.get(); return *this; }
			typename It::value_type::second_type operator*() { return it_->second; }
			MapValueIterator& operator++() { ++it_; return *this;	}
			It get() const { return it_; }
		private:
			It it_;
		};
		struct hash_CoordI {
			size_t operator()(const CoordI& a) const {
				int iHashCode = 17;
				iHashCode = 31 * iHashCode + a.x;
				iHashCode = 31 * iHashCode + a.y;
				iHashCode = 31 * iHashCode + a.z;
				return iHashCode;
			}
		};
		typedef std::unordered_map<CoordI,T,hash_CoordI> container;
		typedef MapValueIterator<typename container::iterator> iterator;
		typedef MapValueIterator<typename container::const_iterator> const_iterator;
		size_t size() const {
			return data_.size();
		}
		iterator begin() {
			return iterator(data_.begin());
		}
		iterator end() {
			return iterator(data_.end());
		}
		const_iterator begin() const {
			return const_iterator(data_.begin());
		}
		const_iterator end() const {
			return const_iterator(data_.end());
		}
		const_iterator find(const CoordI& c) const {
			typename container::const_iterator it = data_.find(c);
			if(it == data_.end() || it->second == 0) {
				return end();
			}
			return const_iterator(it);
		}
		iterator find(const CoordI& c) {
			typename container::iterator it = data_.find(c);
			if(it == data_.end() || it->second == 0) {
				return end();
			}
			return iterator(it);
		}
//		T& operator[](const CoordI& c) {
//			iterator it = find(c);
//			return *it;
//		}
		void set(const CoordI& c, T v) {
			data_[c] = v;
		}
		T operator[](const CoordI& c) const {
			const_iterator it = find(c);
			return *it;
		}
		template<typename It, typename Jt>
		static bool Equals(const MapValueIterator<It>& a, const MapValueIterator<Jt>& b) {
			return a.get() == b.get();
		}
	private:
		container data_;
	};
}}}

/** A container which holds an element of type T for each 3D location */
template<typename T>
struct CellContainerType {
//	typedef Candy::Cubes::Impl::Array3<T,128> Result;
	typedef Candy::Cubes::Impl::StdMap3<T> Result;
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
		return !CellContainer::Equals(cells_.find(c), cells_.end());
	}

	void set(const CoordI& c, T v) {
		boost::interprocess::scoped_lock<boost::mutex> guard(cells_mutex_);
		cells_.set(c, v);
	}

	/**
	 * @brief gets an element if it exists or null if not
	 * @param c cell coordinate of element
	 */
	T get(const CoordI& c) const {
		boost::interprocess::scoped_lock<boost::mutex> guard(cells_mutex_);
		typename CellContainer::const_iterator it = cells_.find(c);
		if(CellContainer::Equals(it, cells_.end())) {
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
		if(CellContainer::Equals(it, cells_.end())) {
			T x = creator();
			cells_.set(c, x);
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
		for(typename CellContainer::iterator it=cells_.begin(); !CellContainer::Equals(it, cells_.end()); ++it) {
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
