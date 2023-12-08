#ifndef _TEXTURES_HPP_
#define _TEXTURES_HPP_

#include <common_header.hpp>

#define MAX_TEXTURES 8
#define TEXTURE_SIZE 8

extern float textures[MAX_TEXTURES][TEXTURE_SIZE][TEXTURE_SIZE];

float getTextureSample(int texture, float u, float v, float boundaryDefault);

#endif
