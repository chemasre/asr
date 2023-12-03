#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Windows.h>
#include <winuser.h>

/**************************************************************
                   
                   A SIMPLE RAYCASTER
   
**************************************************************/

#define SCREEN_HEIGHT2WIDTH 4
#define MAX_SCREEN_HEIGHT 60
#define MIN_SCREEN_HEIGHT 10
#define MAX_SCREEN_WIDTH (MAX_SCREEN_HEIGHT * SCREEN_HEIGHT2WIDTH)

#define SCREEN_RESOLUTION_STEPS 5

#define SCREEN_HEIGHT 29

#define SCREEN_FPS 10

#define MAP_WIDTH 8
#define MAP_HEIGHT 11

#define DEG2RAD (2.0f * PI / 360.0f)
#define PI 3.1415927

#define FOV 30.0f
#define MAX_FOV 30.0f
#define MIN_FOV 10.0f
#define FOV_STEPS 10

#define VIEW_DISTANCE 5.0f
#define MIN_VIEW_DISTANCE 2.0f
#define MAX_VIEW_DISTANCE 10.0f
#define VIEW_DISTANCE_STEPS 10

#define DIRECTIONS 8

#define WALL_STEPS 10
#define GROUND_STEPS 3

// const int windowCornerTopLeft = 218;
// const int windowCornerTopRight = 191;
// const int windowCornerBottomLeft = 192;
// const int windowCornerBottomRight = 217;
// const int windowBorderHorizontal = 196;
// const int windowBorderVertical = 179;
// const int windowDividerTipLeft = 195;
// const int windowDividerTipRight = 180;

const int windowCornerTopLeft = '.';
const int windowCornerTopRight = '.';
const int windowCornerBottomLeft = '`';
const int windowCornerBottomRight = '`';
const int windowBorderHorizontal = '-';
const int windowBorderVertical = '|';
const int windowDividerTipLeft = '|';
const int windowDividerTipRight = '|';

// const int windowCornerTopLeft = '.';
// const int windowCornerTopRight = '.';
// const int windowCornerBottomLeft = '.';
// const int windowCornerBottomRight = '.';
// const int windowBorderHorizontal = '.';
// const int windowBorderVertical = ':';
// const int windowDividerTipLeft = ':';
// const int windowDividerTipRight = ':';


char wallSteps[WALL_STEPS + 1] = "WKMOoL|li-";
char groundSteps[GROUND_STEPS + 1] = "~-.";

char screen[MAX_SCREEN_HEIGHT][MAX_SCREEN_WIDTH + 1];

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
                
float playerPosX;
float playerPosY;
float playerAngle;

char directions[DIRECTIONS] = { '^', '/', '>', '\\', 'V', '/', '<', '\\' };

float rayStep = 0.1f;

float rotateStep = 5.0f;
float moveStep = 0.1f;
float viewDistance;
float fov;

HANDLE consoleHandle;

int screenWidth;
int screenHeight;

char screenLine[MAX_SCREEN_WIDTH + 1];

int frameCounter;

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
        if(x + i >= 0 && x + i < screenWidth)
        {
            screen[y][x + i] = s[i];
        }
        
        i ++;
    }
}

void drawColumn(int x, float wallDistanceFactor)
{
    int h = screenHeight * (1 - wallDistanceFactor);
    
    int wallStep = (int)(wallDistanceFactor * WALL_STEPS);
    if(wallStep >= WALL_STEPS) { wallStep = WALL_STEPS - 1; }
    
    char w = wallSteps[wallStep];
    
    for(int y = 0; y < screenHeight; y ++)
    {
        if(y <= screenHeight/2 + h/2 && y >= screenHeight/2 - h/2)
        {
            screen[y][x] = w; 
        }
        else if(y > screenHeight/2 + h/2)  
        {
            float groundDistanceFactor = 1 - (y - (screenHeight/2.0f)) / (screenHeight/2.0f);
            int groundStep = (int)(groundDistanceFactor * GROUND_STEPS);
            if(groundStep >= GROUND_STEPS) { groundStep = GROUND_STEPS - 1; }
        
            char g = groundSteps[groundStep];
            
            screen[y][x] = g;
            
        }
    }
}

void drawView()
{
    float centerX = (float)screenWidth / 2;
    
    for(int x = 0; x < screenWidth; x ++)
    {        
        float distanceFactor;
        
        float distance = rayCast(playerPosX, playerPosY, playerAngle + (x - centerX) / (float)screenWidth * fov / 2, rayStep, viewDistance);

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

void setScreenCell(int cellX, int cellY, char c)
{
    if(cellX >= 0 && cellX < screenWidth && cellY >= 0 && cellY < screenHeight)
    {
        screen[cellY][cellX] = c;
    }    
}

void fillScreenArea(int areaX, int blankAreaY, int areaWidth, int blankAreaHeight, char c)
{
    for(int y = blankAreaY; y < blankAreaY + blankAreaHeight; y ++)
    {
        for(int x = areaX; x < areaX + areaWidth; x++)
        {
            if(x >= 0 && x < screenWidth && y >= 0 && y < screenHeight)
            {
                screen[y][x] = c;
            }
        }           
       
    }    
}

void drawWindow(int windowX, int windowY, int windowWidth, int windowHeight, char title[])
{
    // Corners
    
    setScreenCell(windowX, windowY, windowCornerTopLeft);
    setScreenCell(windowX + windowWidth - 1, windowY, windowCornerTopRight);
    setScreenCell(windowX, windowY + windowHeight - 1, windowCornerBottomLeft);
    setScreenCell(windowX + windowWidth - 1, windowY + windowHeight - 1, windowCornerBottomRight);
    
    // Horizontal borders

    fillScreenArea(windowX + 1, windowY, windowWidth - 2, 1, windowBorderHorizontal);
    fillScreenArea(windowX + 1, windowY + windowHeight - 1, windowWidth - 2, 1, windowBorderHorizontal);

    int blankAreaY = windowY + 1;
    int blankAreaHeight = windowHeight - 2;
    
    if(title != NULL)
    {
        // Title borders
        
        setScreenCell(windowX, windowY + 1, windowBorderVertical);
        setScreenCell(windowX + windowWidth - 1, windowY + 1, windowBorderVertical);
        
        // Title
        
        fillScreenArea(windowX + 1, windowY + 1, windowWidth - 2, 1, ' ');
        drawString(title, windowX + 2, windowY + 1);
        
        // Title divider tips
        
        setScreenCell(windowX, windowY + 2, windowDividerTipLeft);
        setScreenCell(windowX + windowWidth - 1, windowY + 2, windowDividerTipRight);
        
        // Title divider interior

        fillScreenArea(windowX + 1, windowY + 2, windowWidth - 2, 1, windowBorderHorizontal);
        
        blankAreaY = blankAreaY + 2;
        blankAreaHeight = blankAreaHeight - 2;
    }
    
    // Vertical borders
    
    fillScreenArea(windowX, blankAreaY, 1, blankAreaHeight, windowBorderVertical);
    fillScreenArea(windowX + windowWidth - 1, blankAreaY, 1, blankAreaHeight, windowBorderVertical);
    
    // Blank area

    fillScreenArea(windowX + 1, blankAreaY, windowWidth - 2, blankAreaHeight, ' ');
   
    
    
}



void clearScreen()
{
    for(int y = 0; y < screenHeight; y ++)
    {
        for(int x = 0; x < screenWidth; x++) { screen[y][x] = ' '; }
        screen[y][screenWidth] = '\0';
           
        setScreenCursorPosition(0, y);
        printf(screen[y]);
        
    }

}


void showScreen()
{
    
    for(int y = 0; y < screenHeight; y ++)
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

void initScreen()
{
    consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    
    screenHeight = SCREEN_HEIGHT;
    screenWidth = SCREEN_HEIGHT * SCREEN_HEIGHT2WIDTH;
    
}

void updateScreen()
{
    int increaseResolution = isKeyPressed('4');
    int decreaseResolution = isKeyPressed('3');
    
    if(increaseResolution || decreaseResolution)
    {
        float heightStep = (MAX_SCREEN_HEIGHT - MIN_SCREEN_HEIGHT) / SCREEN_RESOLUTION_STEPS;
        
        if(increaseResolution)
        {
            screenHeight = screenHeight + heightStep;
        }
        else
        {
            screenHeight = screenHeight - heightStep;
        }
        
        if(screenHeight < MIN_SCREEN_HEIGHT) { screenHeight = MIN_SCREEN_HEIGHT; }
        else if(screenHeight > MAX_SCREEN_HEIGHT) { screenHeight = MAX_SCREEN_HEIGHT; }
        
        screenWidth = screenHeight * SCREEN_HEIGHT2WIDTH;
        
    }    
}

void drawHud()
{
    // Stats
    
    drawWindow(screenWidth - 21, 1, 21, 7, NULL);

    sprintf(screenLine, "FOV: %d", (int)fov);    
    drawString(screenLine, screenWidth - 20, 2);
    sprintf(screenLine, "RES: %dx%d", screenWidth, screenHeight);    
    drawString(screenLine, screenWidth - 20, 3);
    sprintf(screenLine, "POS: %.2f,%.2f", playerPosX, playerPosY);    
    drawString(screenLine, screenWidth - 20, 4);
    sprintf(screenLine, "ANG: %.2f(%d)", playerAngle, getPlayerDirection());    
    drawString(screenLine, screenWidth - 20, 5);
    sprintf(screenLine, "DIS: %.2f", viewDistance);    
    drawString(screenLine, screenWidth - 20, 6);
    
    // Minimap
    
    int windowX = screenWidth - 2 - MAP_WIDTH;
    int windowY = screenHeight - 2 - MAP_HEIGHT;
    
    drawWindow(windowX, windowY, MAP_WIDTH + 2, MAP_HEIGHT + 2, NULL);
    
    for(int y = 0; y < MAP_HEIGHT; y ++)
    {
        for(int x = 0; x < MAP_WIDTH; x++)
        {
            screenLine[x] = map[y][x] == 0 ? ' ' : 'W';
        }
        
        screenLine[MAP_WIDTH] = '\0';

        drawString(screenLine, windowX + 1, windowY + 1 + y);
    }
    
    if(frameCounter % 8 < 4) 
    {
        char d = directions[getPlayerDirection()];
        setScreenCell(windowX + 1 + (int)playerPosX, windowY + 1 + (int)playerPosY, d);
    }
    
    
}


void main()
{
    int gameStarted;
    
    gameStarted = 0;
        
    frameCounter = 0;

    initScreen();
    
    while(!isKeyPressed(27))
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
        
        updateScreen();
        
        clearScreen();
        
        if(gameStarted)
        {
            drawView();
            drawHud();
        }
        else
        {
            int x = screenWidth/ 2 - 12;
            int y = screenHeight/2 - 5;
            
            drawWindow(x, y, 24, 8, "A simple Raycaster");
            drawString("   Move/Turn: WASD    ", x + 1, y + 3);
            drawString("   Quit: ESC          ", x + 1, y + 4);
            drawString("                      ", x + 1, y + 5);
            if(frameCounter % 8 < 4) { drawString(" Press space to start ", x + 1, y + 6); }
        }
        
        showScreen();
        
        Sleep((int)(1.0f / SCREEN_FPS * 1000));
        
        frameCounter ++;
        
    }   


}