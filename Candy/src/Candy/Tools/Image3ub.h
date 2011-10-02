#pragma once

#include <boost/shared_array.hpp>
#include <cstring>

struct Image3ub
{
public:
	Image3ub()
	: width_(0), height_(0) {}
	Image3ub(unsigned int width, unsigned int height)
	: width_(width), height_(height) {
		data_.reset(new unsigned char[width_*height_*3]);
	}
	unsigned int width() const {
		return width_;
	}
	unsigned int height() const {
		return height_;
	}
	unsigned char* data() {
		return data_.get();
	}
	const unsigned char* data() const {
		return data_.get();
	}
	unsigned char* at(unsigned int x, unsigned int y) {
		return data() + 3*(x + y*width_);
	}
	const unsigned char* at(unsigned int x, unsigned int y) const {
		return data() + 3*(x + y*width_);
	}
	void set(unsigned int x, unsigned int y, unsigned char cr, unsigned char cg, unsigned char cb) {
		unsigned char* p = at(x, y);
		p[0] = cr;
		p[1] = cg;
		p[2] = cb;
	}
	void fill(unsigned char v) {
		memset(data(), 255, width_*height_*3);
	}
	void fill(unsigned char cr, unsigned char cg, unsigned char cb) {
		unsigned char* p_end = data() + width() * height() * 3;
		for(unsigned char* p=data(); p!=p_end; p+=3) {
			p[0] = cr;
			p[1] = cg;
			p[2] = cb;
		}
	}
private:
	boost::shared_array<unsigned char> data_;
	unsigned int width_, height_;
};

