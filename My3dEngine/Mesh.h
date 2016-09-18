
#ifndef MESH_H
#define MESH_H

#include <cstddef>
#include "Vec.h"
#include "Mat.h"
#include "Quaternion.h"
#include <memory>
#include <Windows.h>

#define PI    (3.1415926535898 / 180)
#define A_PI  (180 / 3.1415926535898)
#define COS15 (float)(0.9659258)
#define BALL_PRECISION  5


namespace g3 {

/**
 * The information we store at the vertex level.
 */
struct Vertex {
  Vec3     pos;
  COLORREF color;
};

struct Triangle {
  unsigned int vertexIndex[3];
  Vec3 normal;
}; // struct Triangle

/**
 * Stores an indexed triangle mesh with N vertices and M triangles.
 */
struct TriangleMesh {
  Vec3   center;
  Vec3   loc;
  double R;
  int    precision;

  unsigned int nVertices;
  std::unique_ptr<Vertex[]> vertices;

  unsigned int nFaces;
  std::unique_ptr<Triangle[]> faces;

  float rotationX, rotationY, rotationZ;
};

void loadCube(TriangleMesh& mesh);
//void loadCube1(g3::TriangleMesh& mesh);
Mat4 getWorldMatrix(TriangleMesh& mesh);
void getNormalVector(TriangleMesh& mesh, unsigned int fnum);

} // namespace g3

#endif // MESH_H
