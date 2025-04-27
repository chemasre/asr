#ifndef _VIEW_HPP_
#define _VIEW_HPP_

#include <common_header.hpp>

#define FOV 55.0f
#define MAX_FOV 160.0f
#define MIN_FOV 10.0f
#define FOV_STEPS 10

#define VIEW_DISTANCE 30.0f
#define MIN_VIEW_DISTANCE 30.0f
#define MAX_VIEW_DISTANCE 100.0f
#define VIEW_DISTANCE_STEPS 10
#define SUNLIGHT_DIRECTION_STEPS 10
#define LIGHT_INTENSITY_STEPS 10

#define MAX_SORTED_SPRITES 100

#define VIEW_NEAR_DISTANCE 0.01f

#define VIEW_WORLD_Z 0.6f

extern float fov;
extern float viewDistance;
extern float sunLightDirection;
extern float sunLightIntensity;
extern float ambientLightIntensity;

void initView();
void updateView();
void drawView();

void setViewportCrop(int right, int bottom);
void addSortedSprite(int sprite, float x, float y, float height);

// private

void drawColumn(int viewportX, float depth, float light);
void drawView();

float getViewHeight();
float getViewAspect();


#endif
