#include <map.hpp>



int map[MAP_HEIGHT][MAP_WIDTH] = {
                                     { 1, 1, 1, 1, 1, 1, 1, 1 },
                                     { 1, 0, 1, 0, 0, 1, 0, 1 },
                                     { 1, 0, 1, 0, 0, 1, 0, 1 },
                                     { 1, 0, 0, 0, 0, 0, 0, 1 },
                                     { 1, 0, 1, 0, 0, 1, 0, 1 },
                                     { 1, 1, 1, 0, 0, 1, 1, 1 },
                                     { 1, 0, 1, 0, 0, 1, 0, 1 },
                                     { 1, 0, 1, 0, 0, 1, 0, 1 },
                                     { 1, 0, 0, 0, 0, 0, 0, 1 },
                                     { 1, 1, 1, 0, 0, 1, 1, 1 },
                                     { 1, 1, 1, 1, 1, 1, 1, 1 },
                                 };


int getMapCell(int x, int y, int boundaryDefault)
{
    if(x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) { return boundaryDefault; }
    else { return map[y][x]; }
}

