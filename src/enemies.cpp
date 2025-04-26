#include "enemies.hpp"
#include <player.hpp>
#include <view.hpp>

float enemy1PosX;
float enemy1PosY;
float enemy2PosX;
float enemy2PosY;

void initEnemies()
{
    enemy1PosX = playerPosX;
    enemy1PosY = playerPosY;
    enemy2PosX = playerPosX + 1.5;
    enemy2PosY = playerPosY;
}

void updateEnemies()
{
    
}

void drawEnemies()
{
    addSortedSprite(2, enemy1PosX, enemy1PosY);
    addSortedSprite(3, enemy2PosX, enemy2PosY);
}
