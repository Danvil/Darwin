#pragma once

#include <Candy/Candy_Dll.h>
#include "Image3ub.h"
#include <vector>

/* http://de.wikipedia.org/wiki/Bresenham-Algorithmus */
template<typename F>
void line(int x0, int y0, int x1, int y1, F f)
{
  int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1; 
  int err = dx+dy, e2; /* error value e_xy */
 
  for(;;){  /* loop */
    f(x0, y0);
    if (x0==x1 && y0==y1) break;
    e2 = 2*err;
    if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
    if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
  }
}

CANDY_API void Plot(
	const std::vector<unsigned int>& data, unsigned int min, unsigned int max,
	unsigned char cr, unsigned char cg, unsigned char cb,
	Image3ub& image
);
