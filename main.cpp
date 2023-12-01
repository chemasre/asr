#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Windows.h>
#include <winuser.h>

/**************************************************************
                   
                   A SIMPLE RAYCASTER
   
**************************************************************/

#define SCREEN_HEIGHT 29
#define SCREEN_WIDTH 119

#define SCREEN_FPS 10

#define MAP_WIDTH 8
#define MAP_HEIGHT 11

#define DEG2RAD (2.0f * PI / 360.0f)
#define PI 3.1415927

#define FOV 60.0f
#define MAX_FOV 60.0f
#define MIN_FOV 30.0f
#define FOV_STEPS 10

#define WALL_STEPS 10
#define GROUND_STEPS 3

char wallSteps[WALL_STEPS + 1] = "WKMOoL|li-";
char groundSteps[GROUND_STEPS + 1] = "~-.";

char screen[SCREEN_HEIGHT][SCREEN_WIDTH + 1];

int map[MAP_HEIGHT][MAP_WIDTH] = {
                                     { 1, 1, 1, 1, 1, 1, 1, 1 },
                                     { 1, 0, 1, 0, 0, 1, 0, 1 },
                                     { 1, 0, 1, 0, 0, 1, 0, 1 },
                                     { 1, 0, 0, 0, 0, 0, 0, 1 },
                                     { 1, 0, 1, 0, 0, 1, 0, 1 },
                                     { 1, 1, 1, 0, 0, 1, 1, 1 },
                                     { 1, 0, 1, 0, 0, 1, 0, 1 },
                                     { 1, 0, 1, 0, 0, 1, 0, 1 },
                                     { 1, 0, 0, 0, 0, 0, 0, 1 },
                                     { 1, 1, 1, 0, 0, 1, 1, 1 },
                                     { 1, 1, 1, 1, 1, 1, 1, 1 },
                                 };
                
float posX;
float posY;
float angle;

float rayStep = 0.1f;

float rotateStep = 5.0f;
float moveStep = 0.2f;
float viewDistance = 5.0f;
float fov;

HANDLE consoleHandle;

char linea[MAP_WIDTH + 1];

int checkCollision(float posX, float posY)
{
    int mapX = (int)posX;
    int mapY = (int)posY;
    
    int result;
    
    if(mapX < 0) { result = 1; }
    else if(mapY < 0) { result = 1; }    
    else if(mapX >= MAP_WIDTH) { result = 1; } 
    else if(mapY >= MAP_HEIGHT) { result = 1; }
    else
    {
        result = map[mapY][mapX];
    }
    
    return result;
}

int isKeyPressed(char k)
{
    return GetKeyState(k) & 0x8000;
}

float rayCast(float posX, float posY, float angle, float rayStep, float rayDistance)
{
    
    float distance = 0;
    int collision = 0;
    
    float sinAngle = sin(angle * DEG2RAD);
    float cosAngle = cos(angle * DEG2RAD);
    
    while(distance <= rayDistance && !collision)
    {
        float rayEndX = posX + cosAngle * distance;
        float rayEndY = posY + sinAngle * distance;
        
        if(checkCollision(rayEndX, rayEndY))
        {
            collision = 1;
        }
        else
        {
            distance = distance + rayStep;

        }
    }
    
    if(collision)
    {
        if(distance > rayDistance)
        {
            distance = rayDistance;
        }

        return distance;
    }
    else
    {
        return -1;
    }
}

void drawString(char s[], int x, int y)
{
    int i;
    i = 0;
    
    while(s[i] != '\0')
    {
        if(x + i >= 0 && x + i < SCREEN_WIDTH)
        {
            screen[y][x + i] = s[i];
        }
        
        i ++;
    }
}

void drawColumn(int x, float wallDistanceFactor)
{
    int h = SCREEN_HEIGHT * (1 - wallDistanceFactor);
    
    int wallStep = (int)(wallDistanceFactor * WALL_STEPS);
    if(wallStep >= WALL_STEPS) { wallStep = WALL_STEPS - 1; }
    
    char w = wallSteps[wallStep];
    
    for(int y = 0; y < SCREEN_HEIGHT; y ++)
    {
        if(y <= SCREEN_HEIGHT/2 + h/2 && y >= SCREEN_HEIGHT/2 - h/2)
        {
            screen[y][x] = w; 
        }
        else if(y > SCREEN_HEIGHT/2 + h/2)  
        {
            float groundDistanceFactor = 1 - (y - (SCREEN_HEIGHT/2.0f)) / (SCREEN_HEIGHT/2.0f);
            int groundStep = (int)(groundDistanceFactor * GROUND_STEPS);
            if(groundStep >= GROUND_STEPS) { groundStep = GROUND_STEPS - 1; }
        
            char g = groundSteps[groundStep];
            
            screen[y][x] = g;
            
        }
    }
}

void drawView()
{
    float centerX = (float)SCREEN_WIDTH / 2;
    
    for(int x = 0; x < SCREEN_WIDTH; x ++)
    {        
        float distanceFactor;
        
        float distance = rayCast(posX, posY, angle + (x - centerX) / (float)SCREEN_WIDTH * fov / 2, rayStep, viewDistance);

        if(distance < 0)
        {
            distanceFactor = 1;
        }
        else
        {
            distanceFactor = distance / viewDistance;
        }
        
        drawColumn(x, distanceFactor);
    }
}

void setScreenCursorPosition(int x, int y)
{
    COORD coord;    
    coord.X = 0;
    coord.Y = y;
    SetConsoleCursorPosition(consoleHandle, coord);    
}

void clearScreen()
{
    for(int y = 0; y < SCREEN_HEIGHT; y ++)
    {
        for(int x = 0; x < SCREEN_WIDTH; x++) { screen[y][x] = ' '; }
        screen[y][SCREEN_WIDTH] = '\0';
           
        setScreenCursorPosition(0, y);
        printf(screen[y]);
        
    }

}

void showScreen()
{
    
    for(int y = 0; y < SCREEN_HEIGHT; y ++)
    {
        setScreenCursorPosition(0, y);
        printf(screen[y]);
    }    
}

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
        angle -= rotateStep;
    }
    else if(rotateRight)
    {
        angle += rotateStep;
    }
    
    if(moveForward || moveBackwards || moveLeft || moveRight)
    {
        
        float nextPosX;
        float nextPosY;

        nextPosX = posX;
        nextPosY = posY;
        
        if(moveForward || moveBackwards)
        {
            float stepForwardX = cos(angle * DEG2RAD) * moveStep;
            float stepForwardY = sin(angle * DEG2RAD) * moveStep;

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
            float stepLateralX = cos((angle + 90) * DEG2RAD) * moveStep;
            float stepLateralY = sin((angle + 90) * DEG2RAD) * moveStep;
            
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
            posX = nextPosX;
            posY = nextPosY;
        }

    }
    
    int increaseFov = isKeyPressed('1');
    int decreaseFov = isKeyPressed('2');
    
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
    
    
        
}

void initPlayer()
{
    posX = 3.5f;
    posY = 9.5f;
    
    angle = -90.0f;
    
    fov = MIN_FOV;
}

void initScreen()
{
    consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
}


void main()
{
    int gameStarted;
    
    gameStarted = 0;
        
    initScreen();
    
    while(!isKeyPressed('Q'))
    {
        if(gameStarted)
        {
            updatePlayer();
        }
        else
        {
            if(isKeyPressed(' '))
            {
                initPlayer();
                gameStarted = 1;
            }
        }
        
        clearScreen();
        
        if(gameStarted)
        {
            drawView();
        }
        else
        {
            int x = SCREEN_WIDTH / 2 - 5;
            int y = SCREEN_HEIGHT/2 - 5;
            drawString("********************", x, y);
            drawString("A Simple Raycaster  ", x, y + 1);
            drawString("********************", x, y + 2);
            drawString("  Move/Turn: WASD   ", x, y + 3);
            drawString("  Quit: Q           ", x, y + 4);
            drawString("                    ", x, y + 5);
            drawString("Press space to start", x, y + 6);
        }
        
        showScreen();
        
        Sleep((int)(1.0f / SCREEN_FPS * 1000));
        
    }   


}