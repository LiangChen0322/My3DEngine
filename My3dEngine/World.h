#ifndef _WORLD_H_
#define _WORLD_H_

#include <Windows.h>
#include <memory>
#include <limits>
#include "Camera.h"
#include "Vertex.h"
#include "Vec.h"
#include "Mesh.h"
#include "Mat.h"

namespace g3 {

class World {
public:
	World(unsigned int w, unsigned int h);

	void  freshFrame(void);
	COLORREF *getBuffer(void);
	// bool croll_event(UINT message);
	bool key_press(char key);

private:
	void clear(void);

	void worldRender(void);
	void renderAxesAndGrid(const Mat4& viewProjMat);
	void renderWireframe(const Mat4& viewProjMat);

	int mapXToWin(float x);
	int mapYToWin(float y);

	void drawPoint(PointWin point);
	void drawLine(PointWin p0, PointWin p1);
	void renderLine(PointWin p0, PointWin p1);
	void triangleRender(PointWin p0, PointWin p1, PointWin p2);

	void renderCube(void);

	/* The width of the screen. */
	unsigned int width;
	unsigned int height;

	COLORREF *frontBuffer;
	std::unique_ptr<float[]> depthBuffer;

	unsigned long targetFrameTime;
	// unsigned long startFrameTime;
	// unsigned long finishFrameTime;

	Camera       camera;
	Light        envLight;
	AmbientLight ambLight;
	TriangleMesh cube;
};

}

#endif
