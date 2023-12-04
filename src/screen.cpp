#include "screen.hpp"
#include <input.hpp>


HANDLE consoleHandle;

int screenWidth;
int screenHeight;

char screenLine[MAX_SCREEN_WIDTH + 1];

char screen[MAX_SCREEN_HEIGHT][MAX_SCREEN_WIDTH + 1];

int frameCounter;

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

    frameCounter ++;
}
