#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include "common_header.hpp"

extern float playerPosX;
extern float playerPosY;
extern float playerAngle;

void initPlayer();
int getPlayerDirection();
void updatePlayer();


#endif
