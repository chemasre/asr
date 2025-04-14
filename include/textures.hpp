#ifndef _TEXTURES_HPP_
#define _TEXTURES_HPP_

#include <common_header.hpp>


#define MAX_TEXTURES 2
#define TEXTURE_SIZE 32

#define TEXTURE_DIRECTORY "textures"
#define TEXTURE_FILENAME_PATTERN "texture%02d.tex"

struct TextureColor
{
    float r;
    float g;
    float b;
};

extern float textures[MAX_TEXTURES][TEXTURE_SIZE][TEXTURE_SIZE][3];

void initTextures();
TextureColor getTextureSample(int texture, float u, float v, TextureColor boundaryDefault);

#endif
