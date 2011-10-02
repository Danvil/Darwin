#pragma once

#include <Candy/Candy_Dll.h>
#include <boost/shared_array.hpp>
#include <string>
#include <stdint.h>

namespace Candy
{
	struct Pixel3ub {
		Pixel3ub() {}
		Pixel3ub(unsigned char r, unsigned char g, unsigned char b)
		: r(r), g(g), b(b) {}
		unsigned char r, g, b;
		uint32_t rgb32() const {
			return r + (g << 8) + (b << 16) + (255 << 24);
		}
		operator uint32_t() const {
			return rgb32();
		}
	};

	inline bool operator==(const Pixel3ub& a, const Pixel3ub& b) {
		return a.rgb32() == b.rgb32();
	}

	inline bool operator<(const Pixel3ub& a, const Pixel3ub& b) {
		return a.rgb32() < b.rgb32();
	}

	struct Image3ub
	{
		Image3ub()
			: width_(0), height_(0) {}

		Image3ub(unsigned int width, unsigned int height)
			: width_(width), height_(height) {
			data_.reset(new unsigned char[3*width_*height_]);
		}

		unsigned int width() const {
			return width_;
		}

		unsigned int height() const {
			return height_;
		}

		boost::shared_array<unsigned char> data() const {
			return data_;
		}

		Pixel3ub pixel(unsigned int x, unsigned int y) const {
			unsigned char* p = data_.get() + 3 * (x + y * width_);
			return Pixel3ub(p[0], p[1], p[2]);
		}

	private:
		unsigned int width_;
		unsigned int height_;
		boost::shared_array<unsigned char> data_;
	};

	CANDY_API Image3ub Load(const std::string& filename);

}
