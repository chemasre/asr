#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include <common_header.hpp>

#define PLAYER_BEEP_FREQUENCY 300
#define PLAYER_BEEP_MILLIS 100

#define PLAYER_ANIM_FPS 20

#define PLAYER_MAX_HEALTH 100.0f

#define PLAYER_HEIGHT 1.3f

#define PLAYER_CONTROL_MODE_CLASSIC 0
#define PLAYER_CONTROL_MODE_MODERN 1

#define CAMERA_FOLLOW_DISTANCE 1.3f
#define CAMERA_FOLLOW_STEPS 10


extern float cameraPosX;
extern float cameraPosY;
extern float playerPosX;
extern float playerPosY;
extern float playerAngle;
extern float playerHealth;

extern int playerControlMode;
extern float mouseSensitivity;

void initPlayer();
int getPlayerDirection();
void updatePlayer();
void drawPlayer();


#endif
