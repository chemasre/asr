#include "enemies.hpp"
#include <player.hpp>
#include <view.hpp>

float enemy1PosX;
float enemy1PosY;
float enemy2PosX;
float enemy2PosY;

void initEnemies()
{
    enemy1PosX = 57;
    enemy1PosY = 34;
    enemy2PosX = 58;
    enemy2PosY = 34;
}

void updateEnemies()
{
    
}

void drawEnemies()
{
    addSortedSprite(2, enemy1PosX, enemy1PosY, ENEMIES_HEIGHT);
    addSortedSprite(3, enemy2PosX, enemy2PosY, ENEMIES_HEIGHT);
}
