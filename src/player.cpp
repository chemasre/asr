#include "player.hpp"
#include <sprites.hpp>
#include <map.hpp>
#include <view.hpp>

float cameraPosX;
float cameraPosY;

float playerPosX;
float playerPosY;
float playerAngle;

int playerControlMode;
float mouseSensitivity;

float rotateSpeed = 200;
float moveSpeed = C2W(2.5f);
float strafeSpeed = C2W(2.5f);
float runMoveMultiplier = 2.0f;
float runStrafeMultiplier = 2.0f;

float playerHealth;

int animFrame;
float animTimer;

void updateCamera()
{
    
    float followDistance = CAMERA_FOLLOW_DISTANCE;
    
    float playerForwardX = cos(playerAngle * DEG2RAD);
    float playerForwardY = sin(playerAngle * DEG2RAD);

    cameraPosX = playerPosX - playerForwardX * followDistance;
    cameraPosY = playerPosY - playerForwardY * followDistance;

    while(MAP_CELL_TYPE(getMapCell(0, W2C(cameraPosX), W2C(cameraPosY), MAKE_MAP_CELL(MAP_CELL_TYPE_WALL, 0))) == MAP_CELL_TYPE_WALL && followDistance > 0)
    {
        followDistance -= CAMERA_FOLLOW_DISTANCE / CAMERA_FOLLOW_STEPS;
        if(followDistance < 0) { followDistance = 0; }

        cameraPosX = playerPosX - playerForwardX * followDistance;
        cameraPosY = playerPosY - playerForwardY * followDistance;
    }
    
}

void drawPlayer()
{
    addSortedSprite(animFrame, playerPosX, playerPosY, PLAYER_HEIGHT);
}

void updatePlayer()
{
    int mouseDeltaX;
    int mouseDeltaY;
    getMouseDelta(&mouseDeltaX, &mouseDeltaY);
    
    int keyLeft = isKeyPressed(KEY_A) || isKeyPressed(KEY_LEFT);
    int keyRight = isKeyPressed(KEY_D) || isKeyPressed(KEY_RIGHT);
    int keyForward = isKeyPressed(KEY_W) || isKeyPressed(KEY_UP);
    int keyBackwards = isKeyPressed(KEY_S) || isKeyPressed(KEY_DOWN);
    int keyStrafe = isKeyPressed(KEY_ALT);
    int keyRun = isKeyPressed(KEY_SHIFT);
    
    float mouseRotateScale = 90.0f / (screenHeight * fontHeight) * (mouseSensitivity - 0.5f + 1.0f);
    float mouseMoveScale = C2W(2.0f) / (screenHeight * fontHeight) * (mouseSensitivity - 0.5f + 1.0f);    
    float mouseStrafeScale = C2W(3.0f) / (screenHeight * fontHeight) * (mouseSensitivity - 0.5f + 1.0f);

    int moveForward = 0;
    int moveBackwards = 0;
    float moveAmount = 0;
    
    int strafeLeft = 0;
    int strafeRight = 0;
    float strafeAmount = 0;
    
    int rotateLeft = 0;
    int rotateRight = 0;
    float rotateAmount = 0;
    
    if(playerControlMode == PLAYER_CONTROL_MODE_CLASSIC)
    {
        moveForward = (mouseDeltaY < 0 || keyForward);
        moveBackwards = (mouseDeltaY > 0 || keyBackwards);
        
        float mouseMoveAmount = fabs(mouseDeltaY) * mouseMoveScale;
        float keyMoveAmount = moveSpeed * (1.0f / SCREEN_FPS);     

        moveAmount = MAX(mouseMoveAmount, keyMoveAmount);
        
        if(!keyStrafe)
        {
            rotateLeft = (mouseDeltaX < 0 || keyLeft);
            rotateRight = (mouseDeltaX > 0 || keyRight);
            
            float mouseRotateAmount = fabs(mouseDeltaX) * mouseRotateScale;
            float keyRotateAmount = rotateSpeed * (1.0f / SCREEN_FPS);
            
            rotateAmount = MAX(mouseRotateAmount, keyRotateAmount);
        }
        else
        {
            strafeLeft = (mouseDeltaX < 0 || keyLeft);
            strafeRight = (mouseDeltaX > 0 || keyRight);

            float mouseStrafeAmount = fabs(mouseDeltaX) * mouseStrafeScale;
            float keyStrafeAmount = strafeSpeed * (1.0f / SCREEN_FPS);
            
            strafeAmount = MAX(mouseStrafeAmount, keyStrafeAmount);

        }
        
    }
    else // playerControlMode == PLAYER_CONTROL_MODE_MODERN
    {
        moveForward = keyForward;
        moveBackwards = keyBackwards;
        
        moveAmount = moveSpeed * (1.0f / SCREEN_FPS);
        
        rotateLeft = (mouseDeltaX < 0);
        rotateRight = (mouseDeltaX > 0);
        
        rotateAmount = fabs(mouseDeltaX) * mouseRotateScale;

        strafeLeft = keyLeft;
        strafeRight = keyRight;
        
        strafeAmount = strafeSpeed * (1.0f / SCREEN_FPS);
        
    }
    
    if(keyRun)
    {
        moveAmount *= runMoveMultiplier;
        strafeAmount *= runStrafeMultiplier;
    }
    

    if(rotateLeft)
    {
        playerAngle -= rotateAmount;
        if(playerAngle < 0) { playerAngle += 360.0f; }
    }
    else if(rotateRight)
    {
        playerAngle += rotateAmount;
        if(playerAngle >= 360.0f) { playerAngle -= 360.0f; }
    }
    
    if(moveForward || moveBackwards || strafeLeft || strafeRight)
    {
        
        float nextPosX;
        float nextPosY;

        nextPosX = playerPosX;
        nextPosY = playerPosY;
        
        if(moveForward || moveBackwards)
        {
            float stepForwardX;
            float stepForwardY;
            
            float cosAngle = cos(playerAngle * DEG2RAD);
            float sinAngle = sin(playerAngle * DEG2RAD);

            stepForwardX = cosAngle * moveAmount;
            stepForwardY = sinAngle * moveAmount;

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

        if(strafeLeft || strafeRight)
        {
            float stepLateralX;
            float stepLateralY;
            
            float cosAngle = cos((playerAngle + 90) * DEG2RAD);
            float sinAngle = sin((playerAngle + 90) * DEG2RAD);
            
            stepLateralX = cosAngle * strafeAmount;
            stepLateralY = sinAngle * strafeAmount;

            if(strafeRight)
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
        else if(nextPosX > C2W(MAP_WIDTH)) { nextPosX = C2W(MAP_WIDTH); }

        if(nextPosY < 0) { nextPosY = 0; }
        else if(nextPosY > C2W(MAP_HEIGHT)) { nextPosY = C2W(MAP_HEIGHT); }
        
        if(MAP_CELL_TYPE(getMapCell(0, W2C(nextPosX), W2C(nextPosY), MAKE_MAP_CELL(MAP_CELL_TYPE_WALL, 0))) != MAP_CELL_TYPE_WALL)
        {
            playerPosX = nextPosX;
            playerPosY = nextPosY;
        }
        else if(MAP_CELL_TYPE(getMapCell(0, W2C(playerPosX), W2C(nextPosY), MAKE_MAP_CELL(MAP_CELL_TYPE_WALL, 0))) != MAP_CELL_TYPE_WALL)
        {
            playerPosY = nextPosY;            
        }
        else if(MAP_CELL_TYPE(getMapCell(0, W2C(nextPosX), W2C(playerPosY), MAKE_MAP_CELL(MAP_CELL_TYPE_WALL, 0))) != MAP_CELL_TYPE_WALL)
        {
            playerPosX = nextPosX;            
        }
        else
        {
            beep(PLAYER_BEEP_FREQUENCY,PLAYER_BEEP_MILLIS);
        }

        
    }
	
	if(moveForward || moveBackwards)
	{
        animTimer += 1.0f / SCREEN_FPS;
		
		if(animTimer > 1.0f / PLAYER_ANIM_FPS) { animFrame = (animFrame + 1) % 2; animTimer -= 1.0f / PLAYER_ANIM_FPS; }    

	}
    
    if(isKeyPressed(KEY_SPACE))
    {
        playerHealth -= 100.0f * 1.0f / PLAYER_ANIM_FPS;
        if(playerHealth < 0) { playerHealth = 0; }
    }
    else
    {
        playerHealth += 60.0f * 1.0f / PLAYER_ANIM_FPS;
        if(playerHealth > PLAYER_MAX_HEALTH) { playerHealth = PLAYER_MAX_HEALTH; }
    }
    

    updateCamera();
}

void initPlayer()
{    

    int startCellX = 0;
    int startCellY = 0;
    for(int x = 0; x < MAP_WIDTH; x++)
    {
        for(int y = 0; y < MAP_HEIGHT; y++)
        {
            if(MAP_CELL_TYPE(maps[0][y][x]) == 2)
            {
                startCellX = x;
                startCellY = y;
            }
        }
        
    }
    
    playerPosX = C2W(startCellX + 0.5f);
    playerPosY = C2W(startCellY + 0.5f);
    
    playerAngle = 270.0f;
 
    updateCamera();

    playerControlMode = PLAYER_CONTROL_MODE_CLASSIC; 
    mouseSensitivity = 0.5f;
	
    playerHealth = PLAYER_MAX_HEALTH;
    
    animFrame = 0;
    animTimer = 0;
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