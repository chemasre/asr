#include "hud.hpp"
#include <screen.hpp>
#include <ui.hpp>
#include <player.hpp>
#include <map.hpp>
#include <view.hpp>

#define DIRECTIONS 8

char directions[DIRECTIONS] = { '^', '/', '>', '\\', 'V', '/', '<', '\\' };

char textBuffer[1024];

int minimapScale;

void initHud()
{
    minimapScale = 2;
}

void updateHud()
{
    int cycleMinimap = isKeyPressed(KEY_M);
    
    if(cycleMinimap)
    {
        minimapScale ++;
        
        if(minimapScale > MINIMAP_MAX_SCALE) { minimapScale = 1; }
    } 
}

void drawCube(int x, int y, int w, int h)
{
    setScreenCell(x, y, MINIMAP_COLOR, '/');
    setScreenCell(x + w - 1, y, MINIMAP_COLOR, '\\');
    setScreenCell(x, y + h - 1, MINIMAP_COLOR, '\\');
    setScreenCell(x + w - 1, y + h - 1, MINIMAP_COLOR, '/');
    
     for(int i = 0; i < w - 2; i++)
    {
        setScreenCell(x + 1 + i, y, MINIMAP_COLOR, '-');
        setScreenCell(x + 1 + i, y + h - 1, MINIMAP_COLOR, '-');
        
        for(int j = y + 1; j < y + h - 1; j++)
        {
            setScreenCell(x + 1 + i, j, MINIMAP_COLOR, 'X');            
        }
        
    }

    for(int i = 0; i < h - 2; i++)
    {
        setScreenCell(x, y + 1 + i, MINIMAP_COLOR, '|');
        setScreenCell(x + w - 1, y + 1 + i, MINIMAP_COLOR, '|');
    }
}

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
    
    
    int playerMinimapPosX = (int)(W2C(playerPosX) * minimapScale);
    int playerMinimapPosY = (int)(W2C(playerPosY) * minimapScale);
    
    int windowX = screenWidth - 2 - MINIMAP_WIDTH;
    int windowY = screenHeight - 2 - MINIMAP_HEIGHT;
    
    drawWindow(windowX, windowY, MINIMAP_WIDTH + 2, MINIMAP_HEIGHT + 2, NULL, HUD_COLOR);    
    
    int minimapX = windowX + 1;
    int minimapY = windowY + 1;    

    enableClipArea();
    
    setClipArea(minimapX, minimapY, MINIMAP_WIDTH, MINIMAP_HEIGHT);
    
    
    int drawMinimapX = minimapX - playerMinimapPosX + MINIMAP_WIDTH / 2;
    int drawMinimapY = minimapY - playerMinimapPosY + MINIMAP_HEIGHT / 2;

    
    drawWindow(drawMinimapX - 1, drawMinimapY - 1, MAP_WIDTH * minimapScale + 2, MAP_HEIGHT * minimapScale + 2, NULL, MINIMAP_COLOR);
    
    for(int y = 0; y < MAP_HEIGHT; y ++)
    {
        for(int x = 0; x < MAP_WIDTH; x++)
        {
            // textBuffer[x] = MAP_CELL_TYPE(maps[0][y][x]) != MAP_CELL_TYPE_WALL ? ' ' : 'W';
            if(MAP_CELL_TYPE(maps[0][y][x]) == MAP_CELL_TYPE_WALL)
            {
                drawCube(drawMinimapX + x * minimapScale, drawMinimapY + y * minimapScale, minimapScale, minimapScale);
            }
        }
        
        //textBuffer[MAP_WIDTH] = '\0';
        
        // drawString(HUD_COLOR, textBuffer, windowX + 1, windowY + 1 + y);
    }
    

    if(frameCounter % 8 < 4) 
    {
        char d = directions[getPlayerDirection()];
        setScreenCell(drawMinimapX + playerMinimapPosX, drawMinimapY + playerMinimapPosY, MINIMAP_PLAYER_COLOR, d);
    }   

    disableClipArea();
    
    
    
}


void drawFloat(int x, int y, char name[], float value)
{
    sprintf(textBuffer, "%s: %.2f", name, value);
    drawString(HUD_COLOR, textBuffer, x, y);
}
