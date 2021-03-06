#include "stdafx.h"
#include "World.h"
#include <iostream>

static int count = 0;

g3::World::World(unsigned int w, unsigned int h):
  width {w},
  height {h}
{
  depthBuffer.reset(new float[width * height]);

  g3::loadCube(cube);
  loadTexImage(texImage);

  frontBuffer = (COLORREF*)malloc(width * height * sizeof(COLORREF));
  camera.eye = Vec3{ 20, 15, 0};
  camera.target = cube.center - camera.eye;
  camera.ratio = 0;
  camera.zoomFactor = 800;
  //camera = Camera { Vec3{20, 25, 0}, Vec3{-20, -23, 0}, 1200, 0};// 1208

  ambLight = {1.0, 1.0, 1.0, 1.0, Vec3{-1, -1, 0}};
  renderCube();
  loadTexImage(texImage);
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

  // Rotates the cube around the y axis in radians.
  // cube.rotationX += 0.01;
  // cube.rotationY += 0.01;
  // cube.rotationZ += 0.01;

  Vec3 upWorld {0, 1, 0};

  Mat4 viewProjMatrix = g3::createLookAtLHMatrix(camera.eye, camera.target, upWorld)
          * g3::createPerspectiveFovLHMatrix(0.78f, width / (float)height, 0.01f, 25.0f);

  renderAxesAndGrid(viewProjMatrix);
  renderWireframe(viewProjMatrix);
  displayTexImage();
}

COLORREF *g3::World::getBuffer(void)
{
	return frontBuffer;
}

inline int g3::World::mapXToWin(float x)
{
  return (int)( x * camera.zoomFactor / (width/(float)height)  ) + (width / 2.0f);
}

inline int g3::World::mapYToWin(float y)
{
  return (int)( -y * camera.zoomFactor ) + (height / 2.0f);
}

/***************************
 * Different event pro
 ***************************/
// bool g3::World::scroll_event(UINT message)
// {
// 	float zoomFactorPercent = 0.05;

// 	if (event->direction == GdkScrollDirection::GDK_SCROLL_UP) {
// 		camera.zoomFactor += camera.zoomFactor * zoomFactorPercent;
// 	} else {
// 		camera.zoomFactor -= camera.zoomFactor * zoomFactorPercent;
// 	}

// 	return TRUE;
// }

bool g3::World::key_press(char key)
{
  float zoomFactorPercent = 0.05;

  switch (key) {
    case 'a':
    case 'A': camera.ratio += 2;
      break;
    case 'd':
    case 'D': camera.ratio -= 2;
      break;
    case 'j':
    case 'J':
      if (camera.zoomFactor < 3000) camera.zoomFactor += camera.zoomFactor * zoomFactorPercent;
      break;
    case 'k':
    case 'K':
      if (camera.zoomFactor > 300) camera.zoomFactor -= camera.zoomFactor * zoomFactorPercent;
      break;
	default: break;
  }
  camera.eye[0] = 20 * std::cos(camera.ratio * PI);
  camera.eye[2] = 20 * std::sin(camera.ratio * PI);
  camera.target = cube.center - camera.eye;

	return TRUE;
}

void g3::World::renderCube(void)
{
  Vec3 dir = ambLight.direction;
  float ldir = std::sqrt(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);

  for (int i = 0; i < cube.nVertices; i++) {
    Vec3 N = cube.vertices[i].pos - cube.center;
    float ln = std::sqrt(N[0] * N[0] + N[1] * N[1] + N[2] * N[2]);

    float cos = -dotProduct(N, ambLight.direction) / (ln * ldir);
    cos = (cos + 1) / 2;

    int r = GetRValue(cube.vertices[i].color) * cos * ambLight.R;
    int g = GetGValue(cube.vertices[i].color) * cos * ambLight.G;
    int b = GetBValue(cube.vertices[i].color) * cos * ambLight.B;

    cube.vertices[i].color = RGB(r, g, b);
    //cube.vertices[i].color = RGB(0xFF, 0xFF, 0xFF);
  }
}

void g3::World::renderAxesAndGrid(const g3::Mat4& viewProjMat)
{
  Mat4 staticMatrix = createScaleMatrix(1) * viewProjMat;

  float step = 1;
  int size = 8; // size X size
  Vec3 *grid = new Vec3[ 4*(size + 1) ];

  Vec3 startX {  size/2.0f*step, 0, size/2.0f*step  };
  for (int n = 0, m = 0; n < (size+1); n++, m+=4) {
    grid[m]   = { startX[0],               0, startX[2] - (n*step) }; 
    grid[m+1] = { startX[0] - (size*step), 0, startX[2] - (n*step) }; 
    grid[m+2] = { startX[0] - (n*step),    0, startX[2] };  
    grid[m+3] = { startX[0] - (n*step),    0, startX[2] - (size*step) };  
  }

  COLORREF gridColor = RGB(205, 201, 201);
  for (int n = 0; n < 4*(size+1); n+=2) {
    Vec3 g1 = transformP3( grid[n], staticMatrix );
    Vec3 g2 = transformP3( grid[n+1], staticMatrix );
    PointWin p0 = {mapXToWin(g1[0]), mapYToWin(g1[1]), g1[2], gridColor};
    PointWin p1 = {mapXToWin(g2[0]), mapYToWin(g2[1]), g2[2], gridColor};
    drawLine(p0, p1);
  }
  delete grid;
}

/**
 * Renders the cube wireframe.
 */
void g3::World::renderWireframe(const g3::Mat4& viewProjMatrix)
{
  Mat4 transformMatrix = g3::getWorldMatrix(cube) * viewProjMatrix;

  for (unsigned int i = 0; i < cube.nFaces; i++) {
    float ans = dotProduct( cube.vertices[ cube.faces[i].vertexIndex[0] ].pos - camera.eye, cube.faces[i].normal);

    if (ans < 0.0) {
      Vec3 v[3];
      int mapToWin[6];
      //unsigned long color[3];
      PointWin pw[3];
      COLORREF black = RGB(0, 0, 0);

      for (unsigned int j = 0; j < 3; j++) {
        v[j] = transformP3( cube.vertices[ cube.faces[i].vertexIndex[j] ].pos, transformMatrix );
        pw[j] = {mapXToWin(v[j][0]), mapYToWin(v[j][1]), v[j][2], black};
      }

      // drawLine(pw[0], pw[1]);
      // drawLine(pw[1], pw[2]);
      // drawLine(pw[2], pw[0]);

      for (unsigned int j = 0; j < 3; j++) {
        pw[j].color = cube.vertices[ cube.faces[i].vertexIndex[j] ].color;
      }
      triangleRender(pw[0], pw[1], pw[2]);
    }
  }
}

/*******************
 * load texture image
 *******************/
void g3::World::loadTexImage(TexImage &image)
{
  image.width = 400;
  image.height = 400;
  image.buffer = (COLORREF *)malloc(400 * 400 * sizeof(COLORREF));

  COLORREF black = RGB(0x33, 0x33, 0x33);
  COLORREF white = RGB(0xDD, 0xDD, 0xDD);
  for (int i = 0; i < 400; i++) {
    int t = i * 400;
    if ((i / 20) % 2 == 0) {
      for (int j = 0; j < 400; j += 40) {
        for (int k = 0; k < 20; k++) { image.buffer[t++] = black; }
        for (int k = 0; k < 20; k++) { image.buffer[t++] = white; }
      }
    } else {
      for (int j = 0; j < 400; j += 40) {
        for (int k = 0; k < 20; k++) { image.buffer[t++] = white; }
        for (int k = 0; k < 20; k++) { image.buffer[t++] = black; }
      }
    }
  }
}

void g3::World::displayTexImage(void)
{
  for (int i = 0; i < 400; i++) {
    int t = i * 400;
	for (int j = 0; j < 400; j++) {
	  drawPoint(PointWin{ i, j, 0.5, texImage.buffer[t++] });
    }
  }
}

int g3::World::mapTexCoord() {
  mapTexTriangle();
}

COLORREF g3::World::mapTexLine(int wx0, int wy0, int wx1, int wy1,
                           int tx0, int ty0, int tx1, int ty1)
{
  int steps = std::abs(wy1 - wy0) > std::abs(wx1 - wx0) ? std::abs(wy1 - wy0) : std::abs(wx1 - wx0);
  float dwx = (float)(wx1 - wx0) / steps;
  float dwy = (float)(wy1 - wy0) / steps;
  float dtx = (float)(ts1 - ts0) / steps;
  float dty = (float)(ty1 - ty0) / steps;

  float wx = wx0;
  float wy = wy0;
  float tx = tx0;
  float ty = ty0;

  for (int i = 0; i < steps; i++) {
    if ((wx >= 0) && (wy >= 0) && (wx < width) && (wy < height)) {
      int win_index = (int)(wy * width + wx);
      int tex_index = (int)(ty * texImag.width + tx);
      frontBuffer[win_index] = texImag[tex_index];
    }
    wx += dwx; wy += dwy;
    tx += dtx; ty += dty;
  }
}

COLORREF g3::World::mapTexTriangle(PointWin p0, PointWin p1, PointWin p2, int num)
{
  // int cTex[3][2];

  // cTex[0][0] = 100;
  // cTex[0][1] = 100;
  // cTex[1][0] = 100;
  // cTex[1][1] = 120;
  // cTex[2][0] = 120;
  // cTex[2][1] = 120;

  int ox = p0.x;
  int oy = p0.y;
  float wx = (float)p1.x;
  float wy = (float)p1.y;
  int steps = std::abs(p2.x - p1.x) > std::abs(p2.y - p1.y) ? std::abs(p2.x - p1.x) : std::abs(p2.y - p1.y);
  float dx = (float)(p2.x - p1.x) / steps;
  float dy = (float)(p2.y - p1.y) / steps;

  float len = std::sqrt((float)((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y)));
  float prop = 0.0;
  for (int i = 0; i < steps; i++) {
    mapTexLine(p0.x, p0.y, wx, wy, 100, 120, 100+20*prop, 120);
    wx += dx;
    wy += dy;
    prop = std::sqrt((float)(wx - p1.x) * (wx - p1.x) + (wy - p1.y) * (wy - p1.y));
    prop = prop / len;
  }
  // int tCoord[3][2];
  // int prec = cube.precision;
  // int angle = 90;
  // int row = num / (360 / prec);
  // int col = num % (360 / prec);

  // int sline = (90 - angle / 2) / prec - 1;
  // if (row < sline) return;
  // int eline = (90 + angle / 2) / prec - 1;
  // if (row > eline) return;
}

/**
 * Draws a point on the screen.
 */
void g3::World::drawPoint(PointWin point)
{
  if ((point.x < 0) || (point.y < 0) || (point.x >= width) || (point.y >= height)) {
    return;
  }
  int targetPixel = point.y * width + point.x;
  if (point.z < depthBuffer[targetPixel]) {
    depthBuffer[targetPixel] = point.z;
  }
  frontBuffer[targetPixel] = point.color;
}

void g3::World::drawLine(PointWin p0, PointWin p1)
{
  if ( ((std::abs(p0.x) > width) && (std::abs(p1.x) > width)) ||
       ((std::abs(p0.y) > height) && (std::abs(p1.y) > height)) ) {
    return;
  }

  float dx = (float)std::abs(p1.x - p0.x);
  float dy = (float)std::abs(p1.y - p0.y);
  float dz = std::abs(p1.z - p0.z);
  int sx = (p0.x < p1.x) ? 1 : -1;
  int sy = (p0.y < p1.y) ? 1 : -1;

  int err = dx - dy;
  float gradient = 0;

  int x = p0.x;
  int y = p0.y;
  float z = p0.z;

  int steps = (dx > dy) ? dx : dy;

  while (true) {
    drawPoint(PointWin {x, y, z, p0.color});

    if ((x == p1.x) && (y == p1.y)) break;
    int e2 = 2 * err;
    if (e2 > -dy) { err -= dy; x += sx; }
    if (e2 < dx) { err += dx; y += sy; }

    // interpolate z depth values
    gradient = (dx > dy) ?  ((x - p0.x) / dx) : ((y - p0.y) / dy);
    z = p0.z + (dz * gradient);
  }
}

void g3::World::renderLine(PointWin p0, PointWin p1)
{
  if ( ((std::abs(p0.x) > width) && (std::abs(p1.x) > width)) ||
       ((std::abs(p0.y) > height) && (std::abs(p1.y) > height)) ) {
    return;
  }

  float dx = (float)std::abs(p1.x - p0.x);
  float dy = (float)std::abs(p1.y - p0.y);
  float dz = std::abs(p1.z - p0.z);
  int sx = (p0.x < p1.x) ? 1 : -1;
  int sy = (p0.y < p1.y) ? 1 : -1;

  int err = dx - dy;
  float gradient = 0;

  int x = p0.x;
  int y = p0.y;
  float z = p0.z;

  int steps = (dx > dy) ? dx : dy;

  float r0 = (float)GetRValue(p0.color); // red
  float g0 = (float)GetGValue(p0.color); // green
  float b0 = (float)GetBValue(p0.color); // blue
  float dr = (float)(GetRValue(p1.color) - r0) / steps; // red
  float dg = (float)(GetGValue(p1.color) - g0) / steps; // green
  float db = (float)(GetBValue(p1.color) - b0) / steps; // blue

  COLORREF color = p0.color;

  while (true) {
    drawPoint(PointWin {x, y, z, color});

    if ((x == p1.x) && (y == p1.y)) break;

    int e2 = 2 * err;
    if (e2 > -dy) { err -= dy; x += sx; }
    if (e2 < dx) { err += dx; y += sy; }

    // interpolate z depth values
    gradient = (dx > dy) ?  ((x - p0.x) / dx) : ((y - p0.y) / dy);
    z = p0.z + (dz * gradient);
    r0 += dr;
    g0 += dg;
    b0 += db;
    color = RGB((int)r0, (int)g0, (int)b0);
  }
}

#define SWAP(a, b)  { a = a + b; b = a - b; a = a - b; }

void g3::World::triangleRender(PointWin p0, PointWin p1, PointWin p2)
{
  COLORREF color[3] = {p0.color, p1.color, p2.color};
  float v[3][3] = {(float)p0.x, (float)p0.y, p0.z, (float)p1.x, (float)p1.y, p1.z, (float)p2.x, (float)p2.y, p2.z};
  int v0 = 0, v1 = 1, v2 = 2;

  float sx0, ex0, dx0;
  float sx1, ex1, dx1;
  float sz0, ez0;
  float sz1, ez1;
  int   sy, ey0, ey1;

  float x0, z0, x1, z1;

  float rgb[3][3];
  float sr0, er0, dr0, sg0, eg0, dg0, sb0, eb0, db0;
  float sr1, er1, dr1, sg1, eg1, dg1, sb1, eb1, db1;

  for (int i = 0; i < 3; i++) {
    rgb[i][0] = (float)GetRValue(color[i]); // red
    rgb[i][1] = (float)GetGValue(color[i]); // blue
    rgb[i][2] = (float)GetBValue(color[i]); // green
  }

  /* sort by y position */
  if (v[v0][1] > v[v1][1]) {
    SWAP(v0, v1);
  }
  if (v[v0][1] > v[v2][1]) {
    SWAP(v0, v2);
  }
  if (v[v1][1] > v[v2][1]) {
    SWAP(v1, v2);
  }

  if (v[v0][1] == v[v1][1]) {
    /*
     *      v2
     *     /  \
     *    /    \
     *   /      \
     *  v0 ---- v1
     */
    sx0 = v[v0][0];
    sx1 = v[v1][0];
    ex0 = ex1 = v[v2][0];
    sy = v[v0][1];
    ey0 = ey1 = v[v2][1];

    sz0 = v[v0][2];
    sz1 = v[v1][2];
    ez0 = ez1 = v[v2][2];

    sr0 = rgb[v0][0]; sg0 = rgb[v0][1]; sb0 = rgb[v0][2];
    sr1 = rgb[v1][0]; sg1 = rgb[v1][1]; sb1 = rgb[v1][2];
    er0 = er1 = rgb[v2][0];
    eg0 = eg1 = rgb[v2][1];
    eb0 = eb1 = rgb[v2][2];
  } else {
    /*
     * v1 ------ v2
     *   \      /
     *    \    /
     *     \  /
     *      v0
     */
    sx0 = sx1 = v[v0][0];
    ex0 = v[v1][0];
    ex1 = v[v2][0];
    sy = v[v0][1];
    ey0 = v[v1][1];
    ey1 = v[v2][1];

    sz0 = sz1 = v[v0][2];
    ez0 = v[v1][2];
    ez1 = v[v2][2];

    er0 = rgb[v1][0]; eg0 = rgb[v1][1]; eb0 = rgb[v1][2];
    er1 = rgb[v2][0]; eg1 = rgb[v2][1]; eb1 = rgb[v2][2];
    sr0 = sr1 = rgb[v0][0];
    sg0 = sg1 = rgb[v0][1];
    sb0 = sb1 = rgb[v0][2];
  }

  float ddx0 = std::abs(ex0 - sx0);
  float ddy0 = std::abs(ey0 - sy);
  float ddz0 = std::abs(ez0 - sz0);
  float ddx1 = std::abs(ex1 - sx1);
  float ddy1 = std::abs(ey1 - sy);
  float ddz1 = std::abs(ez1 - sz1);
  float gradient0, gradient1;

  int steps0 = ey0 - sy;
  int steps1 = ey1 - sy;
  dx0 = (float)(ex0 - sx0) / steps0;
  dx1 = (float)(ex1 - sx1) / steps1;

  dr0 = (er0 - sr0) / steps0;
  dg0 = (eg0 - sg0) / steps0;
  db0 = (eb0 - sb0) / steps0;

  dr1 = (er1 - sr1) / steps1;
  dg1 = (eg1 - sg1) / steps1;
  db1 = (eb1 - sb1) / steps1;

  x0 = sx0;
  x1 = sx1;
  z0 = sz0;
  z1 = sz1;

  COLORREF c0, c1;
  for (int y = sy; y < ey0; y++) {
    c0 = RGB((int)sr0, (int)sg0, (int)sb0);
    c1 = RGB((int)sr1, (int)sg1, (int)sb1);
    renderLine(PointWin{(int)x0, (int)y, z0, c0}, PointWin{(int)x1, (int)y, z1, c1});

    gradient0 = (ddx0 > ddy0) ?  ((x0 - sx0) / ddx0) : ((y - sy) / ddy0);
    z0 = sz0 + (ddz0 * gradient0);
    gradient1 = (ddx1 > ddy1) ?  ((x1 - sx1) / ddx1) : ((y - sy) / ddy1);
    z1 = sz1 + (ddz1 * gradient1);

    x0 += dx0;
    x1 += dx1;

    sr0 += dr0; sg0 += dg0; sb0 += db0;
    sr1 += dr1; sg1 += dg1; sb1 += db1;
  } // end for (int y = sy; y < ey0; y++)

  // if (c0 != color[v1] || c1 != color[v2]) {
  //   int i = 0;
  //   c0 = color[v1];
  // }
  c0 = color[v1];

  if (ey0 != ey1) {
    triangleRender(PointWin{(int)x0, (int)ey0, z0, c0},
                   PointWin{(int)x1, (int)ey0, z1, c1},
                   PointWin{(int)ex1, (int)ey1, ez1, color[v2]});
  }
}
