#ifndef _WORLD_H_
#define _WORLD_H_

#include <Windows.h>
#include <memory>
#include <limits>

namespace g3 {

class World {
public:
	World(unsigned int w, unsigned int h);
	void  freshFrame(void);
	COLORREF *getBuffer(void);

private:
	// void drawPoint(int x, int y, float z, COLORREF color);
	// void drawLine(int x0, int y0, float z0, COLORREF c0, int x1, int y1, float z1, COLORREF c1);
	void drawLine(int x0, int y0, int x1, int y1, COLORREF c1);
	void clear(void);

	/* The width of the screen. */
	unsigned int width;
	unsigned int height;

	COLORREF *frontBuffer;
	std::unique_ptr<float[]> depthBuffer;

	unsigned long targetFrameTime;
	unsigned long startFrameTime;
	unsigned long finishFrameTime;
};

}

#endif
