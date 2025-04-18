#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include <common_header.hpp>

#define PLAYER_BEEP_FREQUENCY 300
#define PLAYER_BEEP_MILLIS 100

#define PLAYER_CONTROL_MODE_CLASSIC 0
#define PLAYER_CONTROL_MODE_MODERN 1

extern float playerPosX;
extern float playerPosY;
extern float playerAngle;

extern int playerControlMode;
extern float mouseSensitivity;

void initPlayer();
int getPlayerDirection();
void updatePlayer();
void drawPlayer();


#endif
