#pragma once

#include "Common.h"
#include "Cube.h"
#include "Impl/LinearKeyValueContainer.h"
#define CHECK_IF_EMPTY_WHEN_MEMCPY

/** A cell of cubes (usually of size 16x16x16)
 * Some data (CubeInteriorData) is stored for every cube in an array.
 * Additional data (CubeSideData) is stored only for border faces.
 */
struct Cell
{
public:
	/** Iterates over all cubes in a cell
	 * Use like this:
	 * for(CellCubeIterator it=cell->IterateCubes(); it.valid(); it.next()) {
	 *    it.type(); // access cube type (type can not be changed)
	 *    it.data(); // access and chance cube data
	 * }
	 */
	struct CubeIterator
	{
	public:
		CubeIterator()
		: _cell(0) {}

		CubeIterator(Cell* cell)
		: _cell(cell), _cc_local(0,0,0), _cc_world(Common::FirstInCell(cell->coordinate())) {
		}

		bool valid() const {
			return _cc_local.z < Common::CellSize;
		}

		void next() {
			assert(valid());
			//if(HasNext()) {
				/*_cc_local.x ++;
				_cc_world.x ++;
				if(_cc_local.x == Common::CellSize) {
					_cc_local.x = 0;
					_cc_local.y ++;
					_cc_world.x -= Common::CellSize;
					_cc_world.y ++;
					if(_cc_local.y == Common::CellSize) {
						_cc_local.y = 0;
						_cc_local.z ++;
						_cc_world.y -= Common::CellSize;
						_cc_world.z ++;
					}
				}*/
				_cc_local.x ++;
				_cc_world.x ++;
				int vx = _cc_local.x & Common::CellSize;
				int dx = vx != 0 ? 1 : 0;
				_cc_local.x -= vx;
				_cc_world.x -= vx;
				_cc_local.y += dx;
				_cc_world.y += dx;
				int vy = _cc_local.y & Common::CellSize;
				int dy = vy != 0 ? 1 : 0;
				_cc_local.y -= vy;
				_cc_world.y -= vy;
				_cc_local.z += dy;
				_cc_world.z += dy;
			//}
		}

		operator bool() const {
			return valid();
		}

		CubeIterator& operator++() {
			next();
			return *this;
		}

		const CoordU& local() const {
			return _cc_local;
		}

		const CoordI& world() const {
			return _cc_world;
		}

		Vec3f positionCenter() const {
			Vec3f p;
			Common::WorldToPositionCenter(_cc_world, p);
			return p;
		}

		CubeType type() const {
			return _cell->At(_cc_local);
		}

		bool IsEmpty() const {
			return type() == CubeTypes::Empty;
		}

		CubeInteriorData* data() const {
			return _cell->AtData(_cc_local);
		}

	private:
		Cell* _cell;
		CoordU _cc_local;
		CoordI _cc_world;
	};

	/** Iterates over all border sides of a cell
	 * Use like this:
	 * for(CellBorderSideIterator it=cubes.IterateBorderSides(cc_cell); it.valid(); it.next()) {
	 *    it.type(); // access cube type (type can not be changed)
	 *    it.data(); // access and chance cube data
	 * }
	 */
	struct BorderSideIterator
	{
	public:
		BorderSideIterator()
		: current_bs_key_(0), current_bs_key_end_(0) {}

		BorderSideIterator(Cell* cell)
		: cell_(cell) {
			current_bs_key_ = cell_->border_sides_->Keys();
			current_bs_key_end_ = current_bs_key_ + cell_->border_sides_->Count();
			current_bs_value_ = cell_->border_sides_->Values();
			if(current_bs_key_ < current_bs_key_end_) {
				Update();
			}
		}

		bool valid() const {
			return current_bs_key_ < current_bs_key_end_;
		}

		void next() {
			assert(valid());
			current_bs_key_++;
			current_bs_value_++;
			Update();
		}

		operator bool() const {
			return valid();
		}

		BorderSideIterator& operator++() {
			next();
			return *this;
		}

		BorderSideId id() const {
			return *current_bs_key_;
		}

		unsigned int side() const {
			return side_;
		}

		const CoordU& local() const {
			return cc_local_;
		}

		const CoordI& world() const {
			return cc_world_;
		}

		Vec3f positionCenter() const {
			return Common::WorldToPositionCenter(cc_world_);
		}

		CubeType type() const {
			return cell_->At(Common::BorderSideToIndex(*current_bs_key_));
		}

		CubeSideData* data() const {
			return current_bs_value_;
		}

	private:
		void Update() {
			Common::BorderSideToLocalSide(*current_bs_key_, cc_local_, side_);
			Common::CellLocalToWorld(cell_->coordinate(), cc_local_, cc_world_);
		}

	private:
		Cell* cell_;
		LinearKeyValueContainer::Key* current_bs_key_;
		LinearKeyValueContainer::Key* current_bs_key_end_;
		LinearKeyValueContainer::Value* current_bs_value_;
		Cu cc_local_;
		Ci cc_world_;
		unsigned int side_;
	};

public:
	Cell(const Ci& c)
		: _coordinate(c)
	{
		data_ = 0;
		is_dirty_ = false;
		_lighting_samples = 0;
		border_sides_ = new LinearKeyValueContainer();
	}

	~Cell() {
		if(data_ != 0) {
			delete[] data_;
			data_ = 0;
		}
	}
	
	bool IsFullyEmpty() const {
		return data_ == 0;
	}

	const LinearKeyValueContainer* GetBorderSides() const {
		return border_sides_;
	}

	bool HasBorder() const {
		return border_sides_->Count() > 0;
	}

	size_t BorderSideCount() const {
		return border_sides_->Count();
	}

	const Ci& coordinate() const {
		return _coordinate;
	}

	bool IsSolid(const CoordU& local) const {
		return At(local) != CubeTypes::Empty;
	}
	
	bool IsEmpty(const CoordU& local) const {
		return At(local) == CubeTypes::Empty;
	}

	CubeType At(const CoordU& local) const {
		if(IsFullyEmpty()) {
			return CubeTypes::Empty;
		} else {
			return data_[Common::LocalToIndex(local)].GetType();
		}
	}

	CubeType At(unsigned int index) const {
		if(IsFullyEmpty()) {
			return CubeTypes::Empty;
		} else {
			return data_[index].GetType();
		}
	}

	CubeInteriorData* AtData(const CoordU& local) const {
		if(IsFullyEmpty()) {
			return 0;
		} else {
			return data_ + Common::LocalToIndex(local);
		}
	}

	CubeInteriorData* AtData(unsigned int index) const {
		if(IsFullyEmpty()) {
			return 0;
		} else {
			return data_ + index;
		}
	}

	void Set(unsigned int index, CubeType type) {
		Set(index, CubeInteriorData::CreateDefault(type));
	}

	void Set(unsigned int index, CubeInteriorData x) {
		if(IsFullyEmpty()) {
			if(x.IsType(CubeTypes::Empty)) {
				return; // nothing to do
			}
			// create cube data memory on the fly
			AllocateImpl();
			// set cube data to empty
			SetAllImpl(CubeInteriorData::CreateEmpty());
		}
		data_[index] = x;
		is_dirty_ = true;
	}

	void Set(const CoordU& local, CubeType type) {
		Set(Common::LocalToIndex(local), type);
	}

	void SetAll(CubeType type) {
		SetAll(CubeInteriorData::CreateDefault(type));
	}

	void SetAll(CubeInteriorData x) {
		if(IsFullyEmpty()) {
			if(x.IsType(CubeTypes::Empty)) {
				return; // nothing to do
			}
			// create cube data on the fly
			AllocateImpl();
		}
		SetAllImpl(x);
		is_dirty_ = true;
	}

	const CubeInteriorData* Begin() const {
		return data_;
	}

	CubeInteriorData* Begin() {
		return data_;
	}

	unsigned int CountLightingSamples() const {
		return _lighting_samples;
	}

	void AddSamples(unsigned int samples) {
		_lighting_samples += samples;
	}

	bool NeedsVitalization() const {
		return is_dirty_;
	}

	CubeIterator IterateCubes() {
		return CubeIterator(this);
	}

	BorderSideIterator IterateBorderSides() {
		return BorderSideIterator(this);
	}

	/** Executes an operation for all cubes in a cell
	 * Op must provide (CoordI cube_world_coordinate, CubeInteriorData* cube_data) -> void
	 */
	template<typename Op>
	void ApplyToAllCubes(Op op) {
		for(Cell::CubeIterator it=IterateCubes(); it.valid(); it.next()) {
			op(it.world(), it.data());
		}
	}

private:
	void AllocateImpl() {
		data_ = new CubeInteriorData[Common::CellCubeCount];
	}

	void SetAllImpl(CubeInteriorData x) {
		//memset(data_, (unsigned char)(type), Common::CellCubeCount);
		for(size_t i=0; i<Common::CellCubeCount; i++) {
			data_[i] = x;
		}
	}

private:
	Ci _coordinate;
	CubeInteriorData* data_;
	bool is_dirty_;
	unsigned int _lighting_samples;

	LinearKeyValueContainer* border_sides_;

	friend class Background;
	friend class Cubes;
	friend struct BorderSideIterator;
};
