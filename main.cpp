#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Windows.h>
#include <winuser.h>

/**************************************************************
                   
                   A SIMPLE RAYCASTER
   
**************************************************************/

#define SCREEN_HEIGHT 20
#define SCREEN_WIDTH 80

#define SCREEN_FPS 10

#define MAP_WIDTH 8
#define MAP_HEIGHT 11

#define DEG2RAD (2.0f * PI / 360.0f)
#define PI 3.1415927

#define FOV 60.0f

#define DISTANCE_STEPS 10

char distanceSteps[DISTANCE_STEPS + 1] = "WKMOoL|li.";

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

HANDLE hStdout;

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

void drawColumn(int x, int h, char c)
{
    for(int y = 0; y < SCREEN_HEIGHT; y ++)
    {
        if(y <= SCREEN_HEIGHT/2 + h/2 && y >= SCREEN_HEIGHT/2 - h/2)
        {
            screen[y][x] = c; 
        }
    }
}

void drawView()
{
    float centerX = (float)SCREEN_WIDTH / 2;
    
    for(int x = 0; x < SCREEN_WIDTH; x ++)
    {
        float distance = rayCast(posX, posY, angle + (x - centerX) / (float)SCREEN_WIDTH * FOV / 2, rayStep, viewDistance);
        
        if(distance >= 0)
        {
            float distanceFactor = distance / viewDistance;
            int distanceStep = (int)(distanceFactor * DISTANCE_STEPS);
            if(distanceStep >= DISTANCE_STEPS) { distanceStep = DISTANCE_STEPS - 1; }
            
            drawColumn(x, SCREEN_HEIGHT * (1 - distanceFactor), distanceSteps[distanceStep]);
        }
    }
}

void setScreenCursorPosition(int x, int y)
{
    COORD coord;    
    coord.X = 0;
    coord.Y = y;
    SetConsoleCursorPosition(hStdout, coord);    
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
    int forward = isKeyPressed('W');
    int backwards = isKeyPressed('S');
    int left = isKeyPressed('A');
    int right = isKeyPressed('D');
    
    if(left)
    {
        angle -= rotateStep;
    }
    else if(right)
    {
        angle += rotateStep;
    }
    
    if(forward || backwards)
    {
        float stepX = cos(angle * DEG2RAD) * moveStep;
        float stepY = sin(angle * DEG2RAD) * moveStep;
        
        float nextPosX;
        float nextPosY;

        nextPosX = posX;
        nextPosY = posY;

        if(forward)
        {
            nextPosX += stepX;
            nextPosY += stepY;
        }
        else
        {
            nextPosX -= stepX;
            nextPosY -= stepY;
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
    
        
}

void initPlayer()
{
    posX = 3.5f;
    posY = 9.5f;
    
    angle = -90.0f;
}

void initScreen()
{
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
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