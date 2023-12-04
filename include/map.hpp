#ifndef _MAP_HPP_
#define _MAP_HPP_

#include <common_header.hpp>

#define FOV 30.0f
#define MAX_FOV 30.0f
#define MIN_FOV 10.0f
#define FOV_STEPS 10

#define VIEW_DISTANCE 5.0f
#define MIN_VIEW_DISTANCE 2.0f
#define MAX_VIEW_DISTANCE 10.0f
#define VIEW_DISTANCE_STEPS 10

#define WALL_STEPS 10
#define GROUND_STEPS 3

#define MAP_WIDTH 8
#define MAP_HEIGHT 11

extern int map[MAP_HEIGHT][MAP_WIDTH];

extern float fov;
extern float viewDistance;

int checkCollision(float posX, float posY);
float rayCast(float posX, float posY, float angle, float rayStep, float rayDistance);
void drawColumn(int x, float wallDistanceFactor);
void drawView();

#endif
