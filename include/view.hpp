#ifndef _VIEW_HPP_
#define _VIEW_HPP_

#include <common_header.hpp>

extern float fov;
extern float viewDistance;

void initView();
void updateView();
void drawView();

// private
int rayCastStep(float prevPosX, float prevPosY, float posX, float posY, float *normal);
int rayCast(float posX, float posY, float angle, float rayStep, float rayDistance, float* hitDistance, float* hitNormal);
void drawColumn(int x, float depht, float light);
void drawView();



#endif
