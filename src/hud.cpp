#include "hud.hpp"
#include <screen.hpp>
#include <ui.hpp>
#include <player.hpp>
#include <map.hpp>
#include <view.hpp>
#include <sprites.hpp>

#define MINIMAP_DIRECTIONS_COUNT 8

char minimapDirections[MINIMAP_DIRECTIONS_COUNT] = { '^', '/', '>', '\\', 'V', '/', '<', '\\' };
int minimapScale;
int minimapEnabled;

int debugInfoEnabled;

#define HEALTH_LEVELS_COUNT 3

float healthLevelsLimits[HEALTH_LEVELS_COUNT] = { 0.5f, 0.9f, 1.0f };
int healthLevelsSprites[HEALTH_LEVELS_COUNT] = { 4, 5, 6 };
int healthLevelsColors[HEALTH_LEVELS_COUNT] = { MAKE_COLOR(0, 255, 0), MAKE_COLOR(255, 255, 0), MAKE_COLOR(255, 0, 0) };


int healthSpritePositionX;
int healthSpritePositionY;

int healthBarWidth;
int healthBarPositionX;

#define INVENTORY_ITEM_WIDTH  4
#define INVENTORY_ITEM_HEIGHT 3
#define INVENTORY_ITEM_COUNT 4

char textBuffer[1024];

char messageBuffer[1024];

void initMinimap()
{
    minimapScale = 1;
    minimapEnabled = 0;
}

void updateMinimap()
{
    int cycleMinimap = isKeyDown(KEY_M);
    
    if(cycleMinimap)
    {
        if(!minimapEnabled)
        {
            minimapEnabled = 1;
            minimapScale = 1;
        }
        else
        {
            minimapScale ++;        
            if(minimapScale > MINIMAP_MAX_SCALE) { minimapEnabled = 0; }
        }
    }     
}

void drawMinimapCube(int x, int y, int w, int h)
{
    setScreenCell(x, y, MINIMAP_COLOR, 'o');
    setScreenCell(x + w - 1, y, MINIMAP_COLOR, 'o');
    setScreenCell(x, y + h - 1, MINIMAP_COLOR, 'o');
    setScreenCell(x + w - 1, y + h - 1, MINIMAP_COLOR, 'o');
    
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

void drawMinimap()
{
    int playerMinimapPosX = (int)(W2C(playerPosX) * minimapScale);
    int playerMinimapPosY = (int)(W2C(playerPosY) * minimapScale);
    
    int windowX = 1;
    int windowY = screenHeight - (HEALTH_HEIGHT + 2) - (MINIMAP_HEIGHT + 2);
    int windowWidth = MINIMAP_WIDTH + 2;
    int windowHeight = MINIMAP_HEIGHT + 2;
    
    drawWindow(windowX, windowY, windowWidth, windowHeight, NULL, HUD_COLOR);    
    
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
                drawMinimapCube(drawMinimapX + x * minimapScale, drawMinimapY + y * minimapScale, minimapScale, minimapScale);
            }
        }
        
        //textBuffer[MAP_WIDTH] = '\0';
        
        // drawString(HUD_COLOR, textBuffer, windowX + 1, windowY + 1 + y);
    }
    

    if(frameCounter % 8 < 4) 
    {
        char d = minimapDirections[getPlayerDirection()];
        setScreenCell(drawMinimapX + playerMinimapPosX, drawMinimapY + playerMinimapPosY, MINIMAP_PLAYER_COLOR, d);
    }   

    disableClipArea();
    
    drawString(HUD_COLOR, "{N}", windowX + windowWidth / 2 - 1, windowY);
    drawString(HUD_COLOR, "{S}", windowX + windowWidth / 2 - 1, windowY + windowHeight - 1);
    drawString(HUD_COLOR, "{W}", windowX - 1, windowY + windowHeight / 2);
    drawString(HUD_COLOR, "{E}", windowX + windowWidth - 1 - 1, windowY + windowHeight / 2);
    

}

void initDebugInfo()
{
    debugInfoEnabled = 0;
}

void updateDebugInfo()
{
    if(isKeyDown(KEY_F1))
    {
        debugInfoEnabled = !debugInfoEnabled;
    }
    
}

void drawFloat(int x, int y, char name[], float value)
{
    sprintf(textBuffer, "%s: %.2f", name, value);
    drawString(HUD_COLOR, textBuffer, x, y);
}


void drawDebugInfo()
{
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
}
    

void updateHealth()
{
    healthSpritePositionX = screenWidth / 2 - HEALTH_WIDTH / 2;
    healthSpritePositionY = 1; //screenHeight - HEALTH_HEIGHT - 1;
    
    healthBarWidth = 1;
    healthBarPositionX = healthSpritePositionX + HEALTH_WIDTH - healthBarWidth;
    
}



void drawHealth()
{
    drawWindow(healthSpritePositionX - 1, healthSpritePositionY - 1, HEALTH_WIDTH + 2, HEALTH_HEIGHT + 2, NULL, HUD_COLOR);

    drawWindow(healthBarPositionX - 1, healthSpritePositionY - 1, healthBarWidth + 2, HEALTH_HEIGHT + 2, NULL, HUD_COLOR);
    
    int healthLevel = -1;
    float normalizedHealth = (1.0f - playerHealth / PLAYER_MAX_HEALTH);
    int i = 0;
    
    while(healthLevel < 0)
    {
        if(i >= HEALTH_LEVELS_COUNT) { healthLevel= HEALTH_LEVELS_COUNT - 1; }
        else if(healthLevelsLimits[i] >= normalizedHealth) {  healthLevel = i; }
        else { i ++; }
    }

    int healthSprite = healthLevelsSprites[healthLevel];
    int healthBarColor = healthLevelsColors[healthLevel];
    
    drawSprite(healthSprite, healthSpritePositionX, healthSpritePositionY);
    
    int healthBarHeight = normalizedHealth * HEALTH_HEIGHT;
    
    fillScreenArea(healthBarPositionX, healthSpritePositionY + HEALTH_HEIGHT - healthBarHeight, 1, healthBarHeight, healthBarColor, '#');
    
}

void drawPlayerAvatar()
{
    //drawSprite(animFrame, screenWidth / 2, screenHeight - 1);    
}

void drawMessage()
{
    int messagePositionX = healthBarPositionX + healthBarWidth + 2;
    int messagePositionY = healthSpritePositionY + 1;
    int messageWidth = screenWidth - messagePositionX - 2;
    int messageHeight = HEALTH_HEIGHT - 2;
    
    drawWindow(messagePositionX - 2, messagePositionY - 2, messageWidth + 4, messageHeight + 4, NULL, HUD_COLOR);
    
    drawTextArea(messagePositionX, messagePositionY, messageWidth, messageHeight, "WHERE'S MOM? SHE PROMISED WE WOULD EAT ICE CREAM TOGETHER",HUD_COLOR);
}

void drawInventory()
{
    int inventoryPositionX = 1;
    int inventoryPositionY = healthSpritePositionY;
    int inventoryWidth = healthSpritePositionX - inventoryPositionX - 1;
    int inventoryHeight = HEALTH_HEIGHT;

    drawWindow(inventoryPositionX - 1, inventoryPositionY - 1, inventoryWidth + 2, inventoryHeight + 2, NULL, HUD_COLOR);
    
    int inventorySeparation = (inventoryWidth - INVENTORY_ITEM_COUNT * INVENTORY_ITEM_WIDTH) / (float)(INVENTORY_ITEM_COUNT + 1);
    
    int itemPositionX = inventorySeparation;
    for(int i = 0; i < INVENTORY_ITEM_COUNT; i++)
    {
        fillScreenArea(itemPositionX, inventoryPositionY + 1, INVENTORY_ITEM_WIDTH, INVENTORY_ITEM_HEIGHT, HUD_COLOR, '.');
        
        itemPositionX += INVENTORY_ITEM_WIDTH + inventorySeparation;
    }
    
}

void initHud()
{
    initDebugInfo();
    initMinimap();
}

void updateHud()
{
    updateDebugInfo();
    updateMinimap();
    updateHealth();
}

void drawHud()
{
    windowCornerTopLeft = '.';
    windowCornerTopRight = '.';
    windowCornerBottomLeft = '.';
    windowCornerBottomRight = '.';
    
    windowBorderHorizontal = '-';
    windowBorderVertical = '|';
    windowDividerTipLeft = '|';
    windowDividerTipRight = '|';
    
    textAreaColor = HUD_TEXT_COLOR;
    windowColor = HUD_COLOR;
    
    enableWindowStyles();    
    
    if(debugInfoEnabled)
    {
        drawDebugInfo();
    }
    
    drawHealth();
    
    if(minimapEnabled)
    {
        drawMinimap();
    }
    
    drawMessage();
    
    drawInventory();
    
    disableWindowStyles();
    
}


