#ifndef _GAME_HPP_
#define _GAME_HPP_

#include <common_header.hpp>

#define MINIMAP_WIDTH 20
#define MINIMAP_HEIGHT 14

#define MINIMAP_MAX_SCALE 3

#define HEALTH_HEIGHT 5
#define HEALTH_WIDTH 13

#define HUD_COLOR MAKE_COLOR(185,122,87)
#define HUD_TEXT_COLOR MAKE_COLOR(172,140,32)

#define MINIMAP_COLOR MAKE_COLOR(255,157,111)
#define MINIMAP_PLAYER_COLOR MAKE_COLOR(255,204,111)

void initHud();
void updateHud();
void drawHud();

// private

void drawFloat(int x, int y, char name[], float value);

#endif
