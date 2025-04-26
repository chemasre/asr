#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include <common_header.hpp>

#define PLAYER_BEEP_FREQUENCY 300
#define PLAYER_BEEP_MILLIS 100

#define PLAYER_ANIM_FPS 20

#define PLAYER_MAX_HEALTH 100.0f

#define PLAYER_CONTROL_MODE_CLASSIC 0
#define PLAYER_CONTROL_MODE_MODERN 1

extern float playerPosX;
extern float playerPosY;
extern float playerAngle;
extern float playerHealth;
extern int playerIsMoving;

extern int playerControlMode;
extern float mouseSensitivity;

void initPlayer();
int getPlayerDirection();
void updatePlayer();
void drawPlayer();


#endif
