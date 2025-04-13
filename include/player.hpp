#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include <common_header.hpp>

#define PLAYER_BEEP_FREQUENCY 300
#define PLAYER_BEEP_MILLIS 100

extern float playerPosX;
extern float playerPosY;
extern float playerAngle;

void initPlayer();
int getPlayerDirection();
void updatePlayer();


#endif
