
#ifndef VERTEX_H
#define VERTEX_H

#include <Windows.h>
#include <memory>

/*
 * Type for three coordinates
 */
struct Vector3 {
  double x, y, z;
};

/*
 * Type for four coordinates
 */
struct Vector4 {
  double x, y, z, w;
};

struct Point3d {
  double x, y, z;
  COLORREF color;
};

struct PointWin {
	int x, y;
	float z;
	COLORREF color;
};

/*
 * The information we store at the vertex level
 */
struct Vertex {
  Vector3 pos;

  //Vertex(): pos{0,0,0} {}
  //Vertex(double x, double y, double z): pos{x,y,z} {}
};

#endif
