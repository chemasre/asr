#include "game.hpp"
#include <screen.hpp>
#include <ui.hpp>
#include <player.hpp>
#include <map.hpp>
#include <view.hpp>

#define DIRECTIONS 8

char directions[DIRECTIONS] = { '^', '/', '>', '\\', 'V', '/', '<', '\\' };

char textBuffer[1024];

void drawHud()
{
    // Stats
    
    drawWindow(screenWidth - 21, 1, 21, 7, NULL, HUD_COLOR);

    sprintf(textBuffer, "FOV: %d", (int)fov);    
    drawString(HUD_COLOR, textBuffer, screenWidth - 20, 2);
    sprintf(textBuffer, "RES: %dx%d", screenWidth, screenHeight);    
    drawString(HUD_COLOR, textBuffer, screenWidth - 20, 3);
    sprintf(textBuffer, "POS: %.2f,%.2f", playerPosX, playerPosY);    
    drawString(HUD_COLOR, textBuffer, screenWidth - 20, 4);
    sprintf(textBuffer, "ANG: %.2f(%d)", playerAngle, getPlayerDirection());    
    drawString(HUD_COLOR, textBuffer, screenWidth - 20, 5);
    sprintf(textBuffer, "DIS: %.2f", viewDistance);    
    drawString(HUD_COLOR, textBuffer, screenWidth - 20, 6);
    
    // Minimap
    
    int windowX = screenWidth - 2 - MAP_WIDTH;
    int windowY = screenHeight - 2 - MAP_HEIGHT;
    
    drawWindow(windowX, windowY, MAP_WIDTH + 2, MAP_HEIGHT + 2, NULL, HUD_COLOR);
    
    for(int y = 0; y < MAP_HEIGHT; y ++)
    {
        for(int x = 0; x < MAP_WIDTH; x++)
        {
            textBuffer[x] = map[y][x] == 0 ? ' ' : 'W';
        }
        
        textBuffer[MAP_WIDTH] = '\0';

        drawString(HUD_COLOR, textBuffer, windowX + 1, windowY + 1 + y);
    }
    
    if(frameCounter % 8 < 4) 
    {
        char d = directions[getPlayerDirection()];
        setScreenCell(windowX + 1 + (int)W2C(playerPosX), windowY + 1 + (int)W2C(playerPosY), HUD_COLOR, d);
    }
    
    
}

void drawFloat(int x, int y, char name[], float value)
{
    sprintf(textBuffer, "%s: %.2f", name, value);
    drawString(HUD_COLOR, textBuffer, x, y);
}
