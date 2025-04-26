#ifndef _SPRITES_HPP_
#define _SPRITES_HPP_

#include <common_header.hpp>


#define MAX_SPRITES 16
#define SPRITE_SIZE 32

#define SPRITE_DIRECTORY "sprites"
#define SPRITE_FILENAME_PATTERN "sprite%02d.spr"

struct SpriteCell
{
    int color;
    char character;
};

struct Sprite
{
    int pivotX;
    int pivotY;
    
    int boundaryX;
    int boundaryY;
    int boundaryWidth;
    int boundaryHeight;
    
    SpriteCell cells[SPRITE_SIZE][SPRITE_SIZE];
};

extern Sprite sprites[MAX_SPRITES];

void initSprites();
void drawSprite(int index, int posX, int posY);



#endif
