#ifndef _VIEW_HPP_
#define _VIEW_HPP_

#include <common_header.hpp>

#define FOV 120.0f
#define MAX_FOV 160.0f
#define MIN_FOV 10.0f
#define FOV_STEPS 10

#define VIEW_DISTANCE 6.0f
#define MIN_VIEW_DISTANCE 2.0f
#define MAX_VIEW_DISTANCE 10.0f
#define VIEW_DISTANCE_STEPS 10
#define SUNLIGHT_DIRECTION_STEPS 10
#define LIGHT_INTENSITY_STEPS 10

extern float fov;
extern float viewDistance;
extern float sunLightDirection;
extern float sunLightIntensity;
extern float ambientLightIntensity;

void initView();
void updateView();
void drawView();

// private
int rayCastStep(float prevPosX, float prevPosY, float posX, float posY, float *normal, float *u);
int rayCast(float posX, float posY, float angle, float rayStep, float rayDistance, float* hitDistance, float* hitNormal, float* hitU);
void drawColumn(int x, float depht, float light);
void drawView();



#endif
