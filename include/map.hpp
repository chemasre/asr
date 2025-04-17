#ifndef _MAP_HPP_
#define _MAP_HPP_

#include <common_header.hpp>

#define MAX_MAPS 16

#define MAP_WIDTH 32
#define MAP_HEIGHT 32

#define MAP_DIRECTORY "maps"
#define MAP_FILENAME_PATTERN "map%02d.map"

#define MAP_CELL_WORLD_SIZE 3.0f

#define W2C(x) ((x) / MAP_CELL_WORLD_SIZE)
#define C2W(x) ((x) * MAP_CELL_WORLD_SIZE)

#define MAP_CELL_TYPE(x) (0xFF & ((x) >> 8))
#define MAP_CELL_PARAM(x) (0xFF & (x))
#define MAKE_MAP_CELL(type,param) (((type) << 8) | 0xFF & (param))

#define MAP_CELL_TYPE_FREE 0 
#define MAP_CELL_TYPE_WALL 1

#define MAP_PARAMS_COUNT 4

extern int maps[MAX_MAPS][MAP_HEIGHT][MAP_WIDTH];
extern int mapParams[MAX_MAPS][MAP_PARAMS_COUNT];

void initMap();

int rayCast(int map, float posX, float posY, float angle, float rayStep, float rayDistance, float* hitDistance, float* hitNormal, int* texure, float* hitU);

int getMapCell(int map, int x, int y, int boundaryDefault);

int getMapParam(int map, int param);

// private
int rayCastStep(int map, float prevPosX, float prevPosY, float posX, float posY, float *normal, int *texture, float *u);

#endif
