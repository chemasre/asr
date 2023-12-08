#include <textures.hpp>



float textures[MAX_TEXTURES][TEXTURE_SIZE][TEXTURE_SIZE] =
{
    {
        { 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f },
        { 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f },
        { 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f },
        { 0.3f, 0.3f, 0.8f, 0.3f, 0.3f, 0.3f, 0.8f, 0.3f },
        { 0.3f, 0.8f, 0.8f, 0.8f, 0.3f, 0.8f, 0.8f, 0.8f },
        { 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f },
        { 0.8f, 0.3f, 0.8f, 0.3f, 0.8f, 0.3f, 0.8f, 0.3f },
        { 0.8f, 0.3f, 0.8f, 0.3f, 0.8f, 0.3f, 0.8f, 0.3f },
    }
};


float getTextureSample(int texture, float u, float v, float boundaryDefault)
{
    if(u < 0 || u >= 1 || v < 0 || v >= 1) { return boundaryDefault; }
    else
    { return textures[texture][(int)(v * TEXTURE_SIZE)][(int)(u * TEXTURE_SIZE)]; }
}

