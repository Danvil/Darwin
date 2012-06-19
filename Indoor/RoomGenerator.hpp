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
		: dim_(0,0,0), pos_(0,0,0), rotate_(0) {}

		Base(const CoordU& dim)
		: dim_(dim), pos_(0,0,0), rotate_(0) {}

		Base(const CoordU& dim, const CoordI& pos)
		: dim_(dim), pos_(pos), rotate_(0) {}

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

		CoordU dim_;
		CoordI pos_;
		unsigned int rotate_;
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
			CubeType t = CubeTypes::Empty;
			for(const Ptr(Base)& x : objs_) {
				if(x->isInside(cw)) {
					t = x->get(cw);
				}
			}
			return t;
		}
	private:
		std::vector<Ptr(Base)> objs_;
	};

	class Box : public Base {
	public:
		virtual ~Box() {}
		Box(const CoordU& dim, CubeType type)
		: Base(dim), type_(type) {}
		Box(const CoordU& dim, CubeType type, const CoordI& pos)
		: Base(dim, pos), type_(type) {}
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

	class Pattern : public Base {
	public:
		Pattern(const CoordU& dim, const CoordI& pos, unsigned int pattern[8][8])
		: Base(dim, pos) {
			for(unsigned int i=0; i<8; i++) {
				for(unsigned int j=0; j<8; j++) {
					pattern_[i][j] = pattern[i][j];
				}
			}
//			std::copy(pattern, pattern+64, pattern_);
		}
		virtual ~Pattern() {}
		CubeType getLocal(const CoordU& cl) {
			unsigned int u = cl.x & 0xF;
			unsigned int v = cl.y & 0xF;
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
//			int pattern[8][8] = {
//					{0, 0, 0, 1, 0, 0, 0, 0},
//					{0, 0, 1, 0, 0, 0, 0, 0},
//					{0, 1, 0, 0, 0, 0, 0, 0},
//					{1, 0, 0, 0, 0, 0, 0, 0},
//					{0, 0, 0, 0, 0, 0, 0, 0},
//					{0, 0, 0, 0, 0, 0, 0, 0},
//					{0, 0, 0, 0, 0, 0, 0, 0},
//					{0, 0, 0, 0, 0, 0, 0, 0},
//			};
			switch(pattern_[u][v]) {
			case 1: return CubeType::EmitGrey1;
			case 2: return CubeType::EmitGrey2;
			case 3: return CubeType::EmitGrey3;
			case 4: return CubeType::EmitGrey4;
			case 5: return CubeType::EmitGrey5;
			case 6: return CubeType::EmitGrey6;
			default: return CubeType::PlainWhite;
			}
		}
	private:
		unsigned int pattern_[8][8];
	};

}

template<unsigned int F=1>
class RoomGenerator
{
public:
	RoomGenerator(const WorldSize& ws)
	: size_(ws) {
		room_contents_.reset(new Rooms::Composite());
		generateRoomOld(ws);
	}

	CubeType operator()(const CoordI& cw_raw)
	{
		CoordI cw(cw_raw.x / F, cw_raw.y / F, cw_raw.z / F);
		// contents
		return room_contents_->get(cw);
	}

private:
	void generateRoomOld(const WorldSize& ws) {
		const unsigned int rsx = (unsigned int)(ws.world_x2() - ws.world_x1() + 1)/F;
		const unsigned int rsy = (unsigned int)(ws.world_y2() - ws.world_y1() + 1)/F;
		const unsigned int rsz = (unsigned int)(ws.world_z2() - ws.world_z1() + 1)/F;
		// floor
		unsigned int pattern[8][8] = {
				{0, 0, 0, 1, 0, 0, 0, 0},
				{0, 0, 1, 0, 0, 0, 0, 0},
				{0, 1, 0, 0, 0, 0, 0, 0},
				{1, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 0},
		};
		room_contents_->add(Ptr(Rooms::Pattern)(new Rooms::Pattern(CoordU(rsx,rsy,1), CoordI(0,0,0), pattern)));
		// walls
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(rsx,rsy,1), CubeTypes::PlainGrey4, CoordI(0,0,rsz-1))));
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(rsx,1,rsz), CubeTypes::PlainGrey4, CoordI(0,0,0))));
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(rsx,1,rsz), CubeTypes::PlainGrey4, CoordI(0,rsy-1,0))));
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(1,rsy,rsz), CubeTypes::PlainGrey4, CoordI(0,0,0))));
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(1,rsy,rsz), CubeTypes::PlainGrey4, CoordI(rsx-1,0,0))));
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
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(1,1,40), CubeTypes::EmitOrange, CoordI(1,1,0))));
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(1,1,40), CubeTypes::EmitOrange, CoordI(78,1,0))));
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(1,1,40), CubeTypes::EmitOrange, CoordI(78,62,0))));
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(1,1,40), CubeTypes::EmitOrange, CoordI(1,62,0))));


	}

	void generateRoomNew(const WorldSize& ws) {
		const unsigned int rsx = (unsigned int)(ws.world_x2() - ws.world_x1() + 1)/F;
		const unsigned int rsy = (unsigned int)(ws.world_y2() - ws.world_y1() + 1)/F;
		const unsigned int rsz = (unsigned int)(ws.world_z2() - ws.world_z1() + 1)/F;
		const unsigned int border_height = 28;
		const unsigned int door_height = 32;
		const unsigned int door_width = 16;
		const unsigned int door_pos = 16;
		const unsigned int door_down = 16;
		// walls
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(rsx,rsy,1), CubeTypes::PlainGrey3, CoordI(0,0,0))));
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(rsx,rsy,1), CubeTypes::PlainGrey4, CoordI(0,0,rsz-1))));
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(rsx,1,rsz), CubeTypes::PlainGrey4, CoordI(0,0,0))));
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(rsx,1,rsz), CubeTypes::PlainGrey4, CoordI(0,rsy-1,0))));
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(1,rsy,rsz), CubeTypes::PlainGrey4, CoordI(0,0,0))));
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(1,rsy,rsz), CubeTypes::PlainGrey4, CoordI(rsx-1,0,0))));
		// door
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(1,door_width,door_height), CubeTypes::AlaMinecraft, CoordI(0,door_pos,0))));
		// some lights
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(4,4,4), CubeTypes::EmitWhite, CoordI(24-2,16-2,0))));
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(4,4,4), CubeTypes::EmitWhite, CoordI(24-2,48-2,0))));
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(4,4,4), CubeTypes::EmitWhite, CoordI(56-2,16-2,0))));
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(4,4,4), CubeTypes::EmitWhite, CoordI(56-2,48-2,0))));
		// border
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(rsx,1,1), CubeTypes::EmitGreen, CoordI(1,1,border_height)))); // south
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(rsx,1,1), CubeTypes::EmitGreen, CoordI(1,rsy-2,border_height)))); // north
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(1,rsy,1), CubeTypes::EmitGreen, CoordI(rsx-2,1,border_height)))); // east
		// west (around the door)
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(1,door_pos-2,1), CubeTypes::EmitGreen, CoordI(1,1,border_height)))); // west
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(1,door_width+4,1), CubeTypes::EmitGreen, CoordI(1,door_pos-2,door_height+1)))); // west
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(1,rsy-(door_pos+door_width+1)-1,1), CubeTypes::EmitGreen, CoordI(1,door_pos+door_width+1,border_height)))); // west
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(1,1,door_height-border_height+1+door_down), CubeTypes::EmitGreen, CoordI(1,door_pos-2,border_height-door_down)))); // west
		room_contents_->add(Ptr(Rooms::Box)(new Rooms::Box(CoordU(1,1,door_height-border_height+1+door_down), CubeTypes::EmitGreen, CoordI(1,door_pos+door_width+1,border_height-door_down)))); // west
	}

private:
	WorldSize size_;
	Ptr(Rooms::Composite) room_contents_;
};

#endif
