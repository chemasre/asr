#include "player.hpp"
#include <input.hpp>
#include <map.hpp>

float playerPosX;
float playerPosY;
float playerAngle;

float rotateStep = 5.0f;
float moveStep = 0.1f;


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
        playerAngle -= rotateStep;
        if(playerAngle < 0) { playerAngle += 360.0f; }
    }
    else if(rotateRight)
    {
        playerAngle += rotateStep;
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
            float stepForwardX = cos(playerAngle * DEG2RAD) * moveStep;
            float stepForwardY = sin(playerAngle * DEG2RAD) * moveStep;

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
            float stepLateralX = cos((playerAngle + 90) * DEG2RAD) * moveStep;
            float stepLateralY = sin((playerAngle + 90) * DEG2RAD) * moveStep;
            
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
        
        if(!checkCollision(nextPosX, nextPosY))
        {
            playerPosX = nextPosX;
            playerPosY = nextPosY;
        }

    }
    
    int increaseFov = isKeyPressed('2');
    int decreaseFov = isKeyPressed('1');
    
    if(increaseFov || decreaseFov)
    {
        float fovStep = (MAX_FOV - MIN_FOV) / FOV_STEPS;
        
        if(increaseFov)
        {
            fov = fov + fovStep;
        }
        else
        {
            fov = fov - fovStep;
        }
        
        if(fov < MIN_FOV) { fov = MIN_FOV; }
        else if(fov > MAX_FOV) { fov = MAX_FOV; }
        
    }
    
    int increaseViewDistance = isKeyPressed('6');
    int decreaseViewDistance = isKeyPressed('5');
    
    if(increaseViewDistance || decreaseViewDistance)
    {
        float viewDistanceStep = (MAX_VIEW_DISTANCE - MIN_VIEW_DISTANCE) / VIEW_DISTANCE_STEPS;
        
        if(increaseViewDistance)
        {
            viewDistance = viewDistance + viewDistanceStep;
        }
        else
        {
            viewDistance = viewDistance - viewDistanceStep;
        }
        
        if(viewDistance < MIN_VIEW_DISTANCE) { viewDistance = MIN_VIEW_DISTANCE; }
        else if(viewDistance > MAX_VIEW_DISTANCE) { viewDistance = MAX_VIEW_DISTANCE; }
        
    }    
    
    
    
        
}

void initPlayer()
{
    playerPosX = 3.5f;
    playerPosY = 9.5f;
    
    playerAngle = 270.0f;
    
    fov = FOV;
    viewDistance = VIEW_DISTANCE;
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