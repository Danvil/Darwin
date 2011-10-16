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

	class Lamp : public Base {
	public:
		virtual ~Lamp() {}
		Lamp() {
			dim_.x = 10;
			dim_.y = 10;
			dim_.z = 7;
		}
		CubeType getLocal(const CoordU& cl) {
			unsigned int x = cl.x;
			unsigned int y = cl.y;
			unsigned int z = cl.z;
			if(z == 6) {
				return CubeTypes::DebugWhite;
			}
			if(z == 0 || z == 1 || z == 2 || z == 3 || z == 4) {
				if(3 <= x && x <= 6 && 3 <= y && y <= 6 && (x + y) % 2 == 0) {
					return CubeTypes::LightWhite;
				}
			}
			if(z == 1 || z == 2 || z == 3 || z == 4 || z == 5) {
				if(cl.x == 0 || cl.x == 9) {
					return (cl.y == 2 || cl.y == 3 || cl.y == 6 || cl.y == 7) ? CubeTypes::Empty : CubeTypes::DebugWhite;
				}
				if(cl.y == 0 || cl.y == 9) {
					return (cl.x == 2 || cl.x == 3 || cl.x == 6 || cl.x == 7) ? CubeTypes::Empty : CubeTypes::DebugWhite;
				}
			}
			return CubeTypes::Empty;
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

template<unsigned int F=1>
class RoomGenerator
{
public:
	RoomGenerator(const WorldSize& ws)
	: size_(ws) {
		room_contents_.reset(new Rooms::Composite());
		// some boxes
		{
			Ptr(Rooms::Box) box(new Rooms::Box(CoordU(14,14,16), CubeTypes::DebugBlue));
			box->pos_ = CoordI(17,17,0);
			room_contents_->add(box);
		}
		{
			Ptr(Rooms::Box) box(new Rooms::Box(CoordU(14,14,16), CubeTypes::DebugGreen));
			box->pos_ = CoordI(33,17,0);
			room_contents_->add(box);
		}
		{
			Ptr(Rooms::Box) box(new Rooms::Box(CoordU(14,14,16), CubeTypes::DebugRed));
			box->pos_ = CoordI(64,48,0);
			room_contents_->add(box);
		}
		// some lights
		{
			Ptr(Rooms::Lamp) lamp(new Rooms::Lamp());
			lamp->pos_ = CoordI(40 - lamp->dim_.x/2, 32 - lamp->dim_.y/2, ws.world_z2()/F - lamp->dim_.z - 3);
			room_contents_->add(lamp);
		}
		{
			Ptr(Rooms::Box) box(new Rooms::Box(CoordU(1,1,40), CubeTypes::EmitOrange));
			box->pos_ = CoordI(1,1,0);
			room_contents_->add(box);
		}
		{
			Ptr(Rooms::Box) box(new Rooms::Box(CoordU(1,1,40), CubeTypes::EmitOrange));
			box->pos_ = CoordI(78,1,0);
			room_contents_->add(box);
		}
		{
			Ptr(Rooms::Box) box(new Rooms::Box(CoordU(1,1,40), CubeTypes::EmitOrange));
			box->pos_ = CoordI(78,62,0);
			room_contents_->add(box);
		}
		{
			Ptr(Rooms::Box) box(new Rooms::Box(CoordU(1,1,40), CubeTypes::EmitOrange));
			box->pos_ = CoordI(1,62,0);
			room_contents_->add(box);
		}

	}

	CubeType operator()(const CoordI& cw_raw)
	{
		bool is_floor = cw_raw.z == size_.world_z1();
		bool is_ceiling = cw_raw.z == size_.world_z2();
		bool is_wall = (cw_raw.x == size_.world_x1() || cw_raw.x == size_.world_x2() || cw_raw.y == size_.world_y1() || cw_raw.y == size_.world_y2());
		CoordI cw(cw_raw.x / F, cw_raw.y / F, cw_raw.z / F);
		if(is_ceiling) {
			return CubeType::DebugWhite;
		}
		if(is_wall) {
			return CubeType::DebugWhite;
		}
		if(is_floor) {
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
//			int pattern[8][8] = {
//					{0, 0, 0, 0, 0, 0, 0, 1},
//					{0, 0, 0, 0, 0, 0, 0, 1},
//					{0, 0, 0, 0, 0, 0, 0, 1},
//					{0, 0, 0, 0, 0, 0, 0, 1},
//					{0, 0, 0, 0, 0, 0, 0, 1},
//					{0, 0, 0, 0, 0, 0, 0, 1},
//					{0, 0, 0, 0, 0, 0, 0, 1},
//					{1, 1, 1, 1, 1, 1, 1, 1},
//			};
			int pattern[8][8] = {
					{0, 0, 0, 1, 0, 0, 0, 0},
					{0, 0, 1, 0, 0, 0, 0, 0},
					{0, 1, 0, 0, 0, 0, 0, 0},
					{1, 0, 0, 0, 0, 0, 0, 0},
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
		// contents
		return room_contents_->get(cw);
	}

private:
	WorldSize size_;
	Ptr(Rooms::Composite) room_contents_;
};

#endif
