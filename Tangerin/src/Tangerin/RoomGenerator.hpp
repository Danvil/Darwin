/*
 * RoomGenerator.hpp
 *
 *  Created on: Oct 13, 2011
 *      Author: david
 */

#ifndef ROOMGENERATOR_HPP_
#define ROOMGENERATOR_HPP_

#include <Candy/Cubes/Generator.h>
#include <vector>

namespace Rooms
{
	class Base
	{
	public:
		Base()
		: pos_(0,0,0), dim_(0,0,0), rotate_(0) {}

		Base(const CoordU& dim)
		: pos_(0,0,0), dim_(dim), rotate_(0) {}

		virtual ~Base() {}

		bool isInside(const CoordI& cw) const {
			CoordI a = cw - pos_;
			return (!(a.x < 0 || a.x >= int(dim_.x) || a.y < 0 || a.y >= int(dim_.y) || a.z < 0 || a.z >= int(dim_.z)));
		}

		CubeType get(const CoordI& cw) {
			CoordI a = cw - pos_;
			if(a.x < 0 || a.x >= int(dim_.x) || a.y < 0 || a.y >= int(dim_.y) || a.z < 0 || a.z >= int(dim_.z)) {
				return CubeType::Empty;
			}
			CoordU b(a.x, a.y, a.z);
			CoordU c;
			switch(rotate_) {
			case 3:
				c.x = dim_.y - 1 - b.y;
				c.y = b.x;
				c.z = b.z;
				break;
			case 2:
				c.x = dim_.x - 1 - b.x;
				c.y = dim_.y - 1 - b.y;
				c.z = b.z;
				break;
			case 1:
				c.x = b.y;
				c.y = dim_.x - 1 - b.x;
				c.z = b.z;
				break;
			default:
			case 0:
				c = b;
				break;
			}
			return getLocal(c);
		}

		virtual CubeType getLocal(const CoordU& cl) = 0;

		CoordI pos_;
		CoordU dim_;
		unsigned int rotate_;
	};

	class Box : public Base {
	public:
		virtual ~Box() {}
		Box(const CoordU& dim, CubeType type)
		: Base(dim), type_(type) {}
		CubeType getLocal(const CoordU& cl) {
			return type_;
		}
	private:
		CubeType type_;
	};

	class Window : public Base {
	public:
		virtual ~Window() {}
		Window() {
			dim_.x = 12;
			dim_.y = 1;
			dim_.z = 20;
		}
		CubeType getLocal(const CoordU& cl) {
			return CubeTypes::LightWhite;
		}
	};

	class Computer : public Base {
	public:
		virtual ~Computer() {}
		Computer() {
			dim_.x = 16;
			dim_.y = 12;
			dim_.z = 20;
		}
		CubeType getLocal(const CoordU& cl) {
			return CubeTypes::DebugBlue;
		}
	};

	class Composite : public Base {
	public:
		virtual ~Composite() {}
		Composite() { }
		void add(const Ptr(Base)& obj) {
			objs_.push_back(obj);
			if(obj->pos_.x < pos_.x) {
				dim_.x += (unsigned int)(pos_.x - obj->pos_.x);
				pos_.x = obj->pos_.x;
			}
			if(obj->pos_.y < pos_.y) {
				dim_.y += (unsigned int)(pos_.y - obj->pos_.y);
				pos_.y = obj->pos_.y;
			}
			if(obj->pos_.z < pos_.z) {
				dim_.z += (unsigned int)(pos_.z - obj->pos_.z);
				pos_.z = obj->pos_.z;
			}
			if(obj->pos_.x + obj->dim_.x > pos_.x + dim_.x) {
				dim_.x += (unsigned int)(obj->pos_.x + obj->dim_.x - pos_.x);
			}
			if(obj->pos_.y + obj->dim_.y > pos_.y + dim_.y) {
				dim_.y += (unsigned int)(obj->pos_.y + obj->dim_.y - pos_.y);
			}
			if(obj->pos_.z + obj->dim_.z > pos_.z + dim_.z) {
				dim_.z += (unsigned int)(obj->pos_.z + obj->dim_.z - pos_.z);
			}
		}
		CubeType getLocal(const CoordU& cl) {
			CoordI cw(int(cl.x), int(cl.y), int(cl.z));
			for(const Ptr(Base)& x : objs_) {
				if(x->isInside(cw)) {
					return x->get(cw);
				}
			}
			return CubeTypes::Empty;
		}
	private:
		std::vector<Ptr(Base)> objs_;
	};
}

class RoomGenerator
{
public:
	RoomGenerator(const WorldSize& ws, unsigned int factor)
	: size_(ws), factor_(factor) {
		room_contents_.reset(new Rooms::Composite());
		// some boxes
		{
			Ptr(Rooms::Box) box(new Rooms::Box(CoordU(16,16,16), CubeTypes::DebugBlue));
			box->pos_ = CoordI(16,16,0);
			room_contents_->add(box);
		}
		{
			Ptr(Rooms::Box) box(new Rooms::Box(CoordU(16,16,16), CubeTypes::DebugGreen));
			box->pos_ = CoordI(32,16,0);
			room_contents_->add(box);
		}
		{
			Ptr(Rooms::Box) box(new Rooms::Box(CoordU(16,16,16), CubeTypes::DebugRed));
			box->pos_ = CoordI(64,48,0);
			room_contents_->add(box);
		}
		// some lights
		{
			Ptr(Rooms::Box) box(new Rooms::Box(CoordU(1,1,32), CubeTypes::LightYellow));
			box->pos_ = CoordI(1,1,8);
			room_contents_->add(box);
		}
		{
			Ptr(Rooms::Box) box(new Rooms::Box(CoordU(1,1,32), CubeTypes::LightYellow));
			box->pos_ = CoordI(78,1,8);
			room_contents_->add(box);
		}
		{
			Ptr(Rooms::Box) box(new Rooms::Box(CoordU(1,1,32), CubeTypes::LightYellow));
			box->pos_ = CoordI(78,62,8);
			room_contents_->add(box);
		}
		{
			Ptr(Rooms::Box) box(new Rooms::Box(CoordU(1,1,32), CubeTypes::LightYellow));
			box->pos_ = CoordI(1,62,8);
			room_contents_->add(box);
		}

	}

	CubeType operator()(const CoordI& cw)
	{
		bool is_floor = cw.z == size_.world_z1();
		bool is_ceiling = cw.z == size_.world_z2();
		bool is_wall = (cw.x == size_.world_x1() || cw.x == size_.world_x2() || cw.y == size_.world_y1() || cw.y == size_.world_y2());
		if(is_floor) {
			return CubeType::DebugWhite;
		}
		if(is_ceiling) {
			unsigned int u = cw.x & 0xF;
			unsigned int v = cw.y & 0xF;
			if(u >= 8) {
				u = 15 - u;
			}
			if(v >= 8) {
				v = 15 - v;
			}
//			int pattern[8][8] = {
//					{0, 0, 0, 1, 0, 0, 0, 1},
//					{0, 0, 1, 0, 0, 0, 0, 1},
//					{0, 1, 0, 0, 0, 0, 0, 1},
//					{1, 0, 0, 0, 0, 0, 0, 1},
//					{0, 0, 0, 0, 0, 0, 0, 1},
//					{0, 0, 0, 0, 0, 0, 0, 1},
//					{0, 0, 0, 0, 0, 0, 0, 1},
//					{1, 1, 1, 1, 1, 1, 1, 1},
//			};
			int pattern[8][8] = {
					{1, 0, 0, 0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0, 0, 0, 0},
			};
			switch(pattern[u][v]) {
			case 1: return CubeType::LightWhite;
			case 2: return CubeType::LightRed;
			case 3: return CubeType::LightGreen;
			case 4: return CubeType::LightBlue;
			default: return CubeType::DebugWhite;
			}
		}
		if(is_wall) {
			return CubeType::DebugWhite;
		}
		// contents
		return room_contents_->get(cw);
	}

private:
	WorldSize size_;
	unsigned int factor_;
	Ptr(Rooms::Composite) room_contents_;
};

#endif
