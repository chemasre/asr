#ifndef _MAP_HPP_
#define _MAP_HPP_

#include <common_header.hpp>

#define MAP_WIDTH 8
#define MAP_HEIGHT 11
#define MAP_CELL_WORLD_SIZE 3.0f

#define W2C(x) ((x) / MAP_CELL_WORLD_SIZE)
#define C2W(x) ((x) * MAP_CELL_WORLD_SIZE)

#define MAP_CELL_FREE 0 
#define MAP_CELL_WALL 1

extern int map[MAP_HEIGHT][MAP_WIDTH];

int rayCast(float posX, float posY, float angle, float rayStep, float rayDistance, float* hitDistance, float* hitNormal, float* hitU);

int getMapCell(int x, int y, int boundaryDefault);

// private
int rayCastStep(float prevPosX, float prevPosY, float posX, float posY, float *normal, float *u);

#endif
