#include "stdafx.h"
#include "World.h"
#include <iostream>

static int count = 0;

g3::World::World(unsigned int w, unsigned int h):
  width {w},
  height {h}
{
	depthBuffer.reset(new float[width * height]);
	frontBuffer = (COLORREF*)malloc(width * height * sizeof(COLORREF));
}

/**
 * Clears the buffers.
 */
void g3::World::clear()
{
	// Fill the buffer with color white
	memset((void *)frontBuffer, 0xDD, width * height * sizeof(COLORREF));
	// Clears the depth buffer
	std::fill(depthBuffer.get(), depthBuffer.get() + (width*height), std::numeric_limits<float>::infinity());
}

void g3::World::freshFrame(void)
{
	clear();
	COLORREF color = RGB(0, 0, 0);
	int t = count % 300;
	drawLine(t, 0, 200 + t, 200, color);
	count += 10;
}

void g3::World::drawLine(int x0, int y0, int x1, int y1, COLORREF c1)
{
	for (int i = x0; i < x1; i++) {
		frontBuffer[i * width + i] = c1;
	}
}

COLORREF *g3::World::getBuffer(void)
{
	return frontBuffer;
}
