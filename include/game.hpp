#ifndef _GAME_HPP_
#define _GAME_HPP_

#include <common_header.hpp>

#define MINIMAP_WIDTH 30
#define MINIMAP_HEIGHT 20

#define MINIMAP_MAX_SCALE 3

#define HUD_COLOR MAKE_COLOR(165,196,248)

#define MINIMAP_COLOR MAKE_COLOR(255,157,111)
#define MINIMAP_PLAYER_COLOR MAKE_COLOR(255,204,111)


void updateHud();
void drawHud();

// private

void drawFloat(int x, int y, char name[], float value);

#endif
