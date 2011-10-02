#include "Plot.h"
#include <boost/utility.hpp>


void Plot(
	const std::vector<unsigned int>& data, unsigned int min, unsigned int max,
	unsigned char cr, unsigned char cg, unsigned char cb,
	Image3ub& image
) {
	unsigned int n = 0;
	unsigned int height = image.height();
	unsigned int width = image.width();
	float scl = float(height)/(float(max) - float(min));
	for(std::vector<unsigned int>::const_reverse_iterator jt=data.rbegin(), kt=boost::next(data.rbegin()); kt!=data.rend() && n < width ; ++jt, ++kt, n++) {
		unsigned int v1 = (unsigned int)((float(*jt) - float(min)) * scl);
		unsigned int v2 = (unsigned int)((float(*kt) - float(min)) * scl);
		if(v1 >= height) {
			v1 = height - 1;
		}
		if(v2 >= height) {
			v2 = height - 1;
		}
		line(n, v1, n+1, v2, [&](int x, int y) {
			image.set(x, y, cr, cg, cb);
		});
	}
}
