#include "player.hpp"
#include <map.hpp>
#include <view.hpp>

float playerPosX;
float playerPosY;
float playerAngle;

float rotateSpeed = 200;
float moveSpeed = 2.5f;
float strafeSpeed = 2.5f;


void updatePlayer()
{
    int strafe = isKeyPressed(VK_MENU);
    int moveForward = isKeyPressed('W');
    int moveBackwards = isKeyPressed('S');
    int left = isKeyPressed('A');
    int right = isKeyPressed('D');    
    int rotateLeft = left && !strafe;
    int rotateRight = right && !strafe;
    int moveLeft = left && strafe;
    int moveRight = right && strafe;

    if(rotateLeft)
    {
        playerAngle -= rotateSpeed * (1.0f / SCREEN_FPS);
        if(playerAngle < 0) { playerAngle += 360.0f; }
    }
    else if(rotateRight)
    {
        playerAngle += rotateSpeed * (1.0f / SCREEN_FPS);
        if(playerAngle >= 360.0f) { playerAngle -= 360.0f; }
    }
    
    if(moveForward || moveBackwards || moveLeft || moveRight)
    {
        
        float nextPosX;
        float nextPosY;

        nextPosX = playerPosX;
        nextPosY = playerPosY;
        
        if(moveForward || moveBackwards)
        {
            float stepForwardX = cos(playerAngle * DEG2RAD) * moveSpeed * (1.0f / SCREEN_FPS);;
            float stepForwardY = sin(playerAngle * DEG2RAD) * moveSpeed * (1.0f / SCREEN_FPS);;

            if(moveForward)
            {
                nextPosX += stepForwardX;
                nextPosY += stepForwardY;
            }
            else
            {
                nextPosX -= stepForwardX;
                nextPosY -= stepForwardY;
            }
        }

        
        if(moveLeft || moveRight)
        {
            float stepLateralX = cos((playerAngle + 90) * DEG2RAD) * strafeSpeed * (1.0f / SCREEN_FPS);;
            float stepLateralY = sin((playerAngle + 90) * DEG2RAD) * strafeSpeed * (1.0f / SCREEN_FPS);;
            
            if(moveRight)
            {
                nextPosX += stepLateralX;
                nextPosY += stepLateralY;
            }
            else
            {
                nextPosX -= stepLateralX;
                nextPosY -= stepLateralY;
            }            
        }
        
        
        if(nextPosX < 0) { nextPosX = 0; }
        else if(nextPosX > MAP_WIDTH) { nextPosX = MAP_WIDTH; }

        if(nextPosY < 0) { nextPosY = 0; }
        else if(nextPosY > MAP_HEIGHT) { nextPosY = MAP_HEIGHT; }
        
        if(getMapCell(nextPosX, nextPosY, MAP_CELL_WALL) == MAP_CELL_FREE)
        {
            playerPosX = nextPosX;
            playerPosY = nextPosY;
        }

    }
    
    
        
}

void initPlayer()
{
    playerPosX = 3.5f;
    playerPosY = 9.5f;
    
    playerAngle = 270.0f;
    
}

int getPlayerDirection()
{
    float sectorAngleWidth = 360.0f / 8;
    float sectorAngle = (playerAngle - (270 - sectorAngleWidth / 2)) / sectorAngleWidth;

    int sector = (int)sectorAngle;
    if(sector < 0) { sector += 8; }
    else if(sector >= 8) { sector -= 8; }
    
    return sector;
    
}