#ifndef _MAP_HPP_
#define _MAP_HPP_

#include <common_header.hpp>

#define MAP_WIDTH 8
#define MAP_HEIGHT 11

#define MAP_CELL_FREE 0 
#define MAP_CELL_WALL 1

extern int map[MAP_HEIGHT][MAP_WIDTH];

int getMapCell(int x, int y, int boundaryDefault);

#endif
