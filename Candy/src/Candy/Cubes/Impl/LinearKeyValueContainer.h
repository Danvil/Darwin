/*
 * LinearKeyValueContainer.h
 *
 *  Created on: 09.08.2011
 *      Author: david
 */

#ifndef LINEARKEYVALUECONTAINER_H_
#define LINEARKEYVALUECONTAINER_H_

#include <list>
#include <vector>
#include <cassert>
#include <cstring>

/** A special list of (key,value) pairs which is kept in a sorted order */
struct LinearKeyValueContainer
{
	typedef BorderSideId Key;
	typedef CubeSideData Value;
	typedef unsigned int Index;

	LinearKeyValueContainer()
	: count_(0), reserved_(0), keys_(0), values_(0) {
	}

	LinearKeyValueContainer(Index count)
	: count_(0), reserved_(count) {
		keys_ = new Key[reserved_];
		values_ = new Value[reserved_];
	}

	LinearKeyValueContainer(const LinearKeyValueContainer& rhs) {
		keys_ = 0;
		values_ = 0;
		ReserveImpl(rhs.count_);
		std::memcpy(keys_, rhs.keys_, count_ * sizeof(Key));
		std::memcpy(values_, rhs.values_, count_ * sizeof(Key));
		count_ = rhs.count_;
	}

	LinearKeyValueContainer& operator=(const LinearKeyValueContainer& rhs) {
		if(this != &rhs) {
			Purge();
			ReserveImpl(rhs.count_);
			std::memcpy(keys_, rhs.keys_, count_ * sizeof(Key));
			std::memcpy(values_, rhs.values_, count_ * sizeof(Key));
			count_ = rhs.count_;
		}
		return *this;
	}

	~LinearKeyValueContainer() {
		Purge();
	}

	/** Sets size to 0 and *does not* free memory */
	void Clear() {
		count_ = 0;
	}

	/** Sets size to 0 and frees memory */
	void Purge() {
		count_ = 0;
		reserved_ = 0;
		if(keys_ != 0) {
			delete[] keys_;
			keys_ = 0;
		}
		if(values_ != 0) {
			delete[] values_;
			values_ = 0;
		}
	}

	/** Assures that the list can store a given number of elements without reallovation */
	void Reserve(Index count) {
		if(count < reserved_) {
			ReserveImpl(count);
		}
	}

	/** Adds many keys to the list potentially reallocating memory */
	void Add(std::list<Key>::const_iterator a, size_t n) {
		if(count_ + n >= reserved_) {
			ReserveImpl(count_ + n);
		}
		Key* p_keys_start = keys_ + count_;
		Key* p_keys_end = p_keys_start + n;
		for(Key* p_keys=p_keys_start; p_keys!=p_keys_end; ++a, ++p_keys) {
			*p_keys = *a;
		}
		count_ += n;
	}

	void Add(std::vector<Key>::const_iterator a, std::vector<Key>::const_iterator b) {
		size_t n = b - a;
		if(count_ + n >= reserved_) {
			ReserveImpl(count_ + n);
		}
		Key* p_keys_start = keys_ + count_;
		Key* p_keys_end = p_keys_start + n;
		for(Key* p_keys=p_keys_start; p_keys!=p_keys_end; ++a, ++p_keys) {
			*p_keys = *a;
		}
		count_ += n;
	}

	/** Adds a given key to the list potentially reallocating memory */
	Value* Add(Key key) {
		assert(count_ == 0 || key > keys_[count_ - 1]);
		if(count_ == reserved_) {
			// need more space
			if(reserved_ < 4) {
				reserved_ = 4;
			} else {
				reserved_ *= 2;
			}
			ReserveImpl(reserved_);
		}
		assert(count_ < reserved_);
		keys_[count_] = key;
		count_ ++;
		return &(values_[count_ - 1]);
	}

	/** Checks if a given key is contained in the list */
	bool HasKey(Key key) {
		return Get(key) != 0;
	}

	/** Gets value for a given key. This operation is O(log n) */
	Value* Get(Key x) {
		if(count_ <= 0) {
			return 0;
		}
		// search first key not smaller than x
		// TODO assumes sorted keys!
		Key* v = std::lower_bound(keys_, keys_ + count_, x);
		size_t index = v - keys_;
		if(index < count_ && *v == x) {
			// found key in list
			return &(values_[index]);
		}
		else {
			// not found
			return 0;
		}
	}

	/** Number of elements */
	Index Count() const {
		return count_;
	}

	Key* Keys() const {
		return keys_;
	}

	Value* Values() const {
		return values_;
	}

	/** The i-th key */
	Key KeyAt(Index i) const {
		return keys_[i];
	}

	/** The i-th value */
	Value* ValueAt(Index i) {
		return &(values_[i]);
	}

	const Value* ValueAt(Index i) const {
		return &(values_[i]);
	}

private:
	void ReserveImpl(size_t r) {
		Key* keys_old = keys_;
		Value* values_old = values_;
		reserved_ = r;
		keys_ = new Key[reserved_];
		values_ = new Value[reserved_];
		if(keys_old != 0) {
			std::memcpy((void*)keys_, (const void*)keys_old, ((size_t)count_) * sizeof(Key));
			delete[] keys_old;
		}
		if(values_old != 0) {
			std::memcpy((void*)values_, (const void*)values_old, ((size_t)count_) * sizeof(Value));
			delete[] values_old;
		}
	}

private:
	Index count_;
	Index reserved_;
	Key* keys_;
	Value* values_;
};

#endif
