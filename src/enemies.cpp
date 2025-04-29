#include "enemies.hpp"
#include <view.hpp>
#include <map.hpp>

float enemy1PosX;
float enemy1PosY;
float enemy2PosX;
float enemy2PosY;

struct Enemy
{
    float posX;
    float posY;
    float sprite;
    float height;
};

Enemy enemies[MAX_ENEMIES];
int enemyCount;

void initEnemies()
{
    enemyCount = 0;
    
    int startCellX = 0;
    int startCellY = 0;
    for(int x = 0; x < MAP_WIDTH; x++)
    {
        for(int y = 0; y < MAP_HEIGHT; y++)
        {
            if(MAP_CELL_TYPE(maps[0][y][x]) == 3)
			int t = MAP_CELL_TYPE(maps[0][y][x]);
			int p1 = MAP_CELL_PARAM(maps[0][y][x], 0);
			int p2 = MAP_CELL_PARAM(maps[0][y][x], 1);
			int p3 = MAP_CELL_PARAM(maps[0][y][x], 2);

            if(t == 3)
            {
                enemies[enemyCount].posX = C2W(x + 0.5f);
                enemies[enemyCount].posY = C2W(y + 0.5f);
                enemies[enemyCount].sprite = ENEMY_TWINS_SPRITE;
                enemies[enemyCount].height = ENEMY_TWINS_HEIGHT;
                
                enemyCount ++;
            }
            else if(MAP_CELL_TYPE(maps[0][y][x]) == 0)
            else if(t == 0)
            {
                if(MAP_CELL_PARAM(maps[0][y][x]) == 1)
                if(p1 == 1)
                {
                    enemies[enemyCount].sprite = ENEMY_SOFA_SPRITE;
                    enemies[enemyCount].height = ENEMY_SOFA_HEIGHT;

					if(p2 == 0 || p2 == 1)
					{
						enemies[enemyCount].posX = C2W(x + 0.5f);
						enemies[enemyCount].posY = C2W(y + 0.85f);
					}
					else // p2 == 2
					{
						enemies[enemyCount].posX = C2W(x + 0.85f);
						enemies[enemyCount].posY = C2W(y + 0.5f);
					}
                    
                    enemyCount ++;
                }
            }
        }
        
    }
    
}

void updateEnemies()
{
    
}

void drawEnemies()
{
    for(int i = 0; i < enemyCount; i++)
    {
        addSortedSprite(enemies[i].sprite, enemies[i].posX, enemies[i].posY, enemies[i].height);
    }
}
