#include <ui.hpp>
#include <player.hpp>
#include <map.hpp>
#include <view.hpp>




#define DIRECTIONS 8

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


char directions[DIRECTIONS] = { '^', '/', '>', '\\', 'V', '/', '<', '\\' };


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