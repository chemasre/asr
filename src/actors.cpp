#include "actors.hpp"
#include <view.hpp>
#include <map.hpp>

#define ACTOR_TWINS_SPRITE 2
#define ACTOR_TWINS_HEIGHT 1.5f

#define ACTOR_SOFA_SPRITE 7
#define ACTOR_SOFA_HEIGHT 1.8f
#define ACTOR_BIN_SPRITE 8
#define ACTOR_BIN_HEIGHT 1.2f
#define ACTOR_POT_SPRITE 9
#define ACTOR_POT_HEIGHT 1.8f
#define ACTOR_TREEPOT_SPRITE 10
#define ACTOR_TREEPOT_HEIGHT 2.0f
#define ACTOR_LAMP1_SPRITE 11
#define ACTOR_LAMP1_HEIGHT 2.5f
#define ACTOR_LAMP1_POSZ 2.5f
#define ACTOR_LAMP2_SPRITE 12
#define ACTOR_LAMP2_HEIGHT 2.5f
#define ACTOR_LAMP2_POSZ 2.5f

struct Actor
{
    float posX;
    float posY;
    float posZ;
    float sprite;
    float height;
};

Actor actors[MAX_ACTORS];
int actorCount;

void initActors()
{
    actorCount = 0;
    
    int startCellX = 0;
    int startCellY = 0;
    for(int x = 0; x < MAP_WIDTH; x++)
    {
        for(int y = 0; y < MAP_HEIGHT; y++)
        {
			int t = MAP_CELL_TYPE(maps[0][y][x]);
			int p1 = MAP_CELL_PARAM(maps[0][y][x], 0);
			int p2 = MAP_CELL_PARAM(maps[0][y][x], 1);
			int p3 = MAP_CELL_PARAM(maps[0][y][x], 2);

            if(t == 3)
            {
                actors[actorCount].posX = C2W(x + 0.5f);
                actors[actorCount].posY = C2W(y + 0.5f);
                actors[actorCount].posZ = 0;
                actors[actorCount].sprite = ACTOR_TWINS_SPRITE;
                actors[actorCount].height = ACTOR_TWINS_HEIGHT;
                
                actorCount ++;
            }
            else if(t == 0)
            {
                if(p1 == 1)
                {
                    actors[actorCount].sprite = ACTOR_SOFA_SPRITE;
                    actors[actorCount].height = ACTOR_SOFA_HEIGHT;
                    actors[actorCount].posZ = 0;

					if(p2 == 0 || p2 == 1)
					{
						actors[actorCount].posX = C2W(x + 0.5f);
						actors[actorCount].posY = C2W(y + 0.95f);
					}
					else // p2 == 2
					{
						actors[actorCount].posX = C2W(x + 0.95f);
						actors[actorCount].posY = C2W(y + 0.5f);
					}
                    
                    actorCount ++;
                }
                else if(p1 == 2)
                {
                    actors[actorCount].sprite = ACTOR_BIN_SPRITE;
                    actors[actorCount].height = ACTOR_BIN_HEIGHT;
                    actors[actorCount].posZ = 0;

					if(p2 == 0 || p2 == 1)
					{
						actors[actorCount].posX = C2W(x + 0.5f);
						actors[actorCount].posY = C2W(y + 0.95f);
					}
					else // p2 == 2
					{
						actors[actorCount].posX = C2W(x + 0.95f);
						actors[actorCount].posY = C2W(y + 0.5f);
					}
                    
                    actorCount ++;
                }                
                else if(p1 == 3)
                {
                    actors[actorCount].sprite = ACTOR_POT_SPRITE;
                    actors[actorCount].height = ACTOR_POT_HEIGHT;
                    actors[actorCount].posZ = 0;

					if(p2 == 0 || p2 == 1)
					{
						actors[actorCount].posX = C2W(x + 0.5f);
						actors[actorCount].posY = C2W(y + 0.95f);
					}
					else // p2 == 2
					{
						actors[actorCount].posX = C2W(x + 0.95f);
						actors[actorCount].posY = C2W(y + 0.5f);
					}
                    
                    actorCount ++;
                }                
                else if(p1 == 4)
                {
                    actors[actorCount].sprite = ACTOR_TREEPOT_SPRITE;
                    actors[actorCount].height = ACTOR_TREEPOT_HEIGHT;
                    actors[actorCount].posZ = 0;

					if(p2 == 0 || p2 == 1)
					{
						actors[actorCount].posX = C2W(x + 0.5f);
						actors[actorCount].posY = C2W(y + 0.95f);
					}
					else // p2 == 2
					{
						actors[actorCount].posX = C2W(x + 0.95f);
						actors[actorCount].posY = C2W(y + 0.5f);
					}
                    
                    actorCount ++;
                } 
                else if(p1 == 5)
                {
                    actors[actorCount].sprite = ACTOR_LAMP1_SPRITE;
                    actors[actorCount].height = ACTOR_LAMP1_HEIGHT;
                    actors[actorCount].posZ = ACTOR_LAMP1_POSZ;

                    actors[actorCount].posX = C2W(x + 0.5f);
                    actors[actorCount].posY = C2W(y + 0.5f);
                    
                    actorCount ++;
                }
                else if(p1 == 6)
                {
                    actors[actorCount].sprite = ACTOR_LAMP2_SPRITE;
                    actors[actorCount].height = ACTOR_LAMP2_HEIGHT;
                    actors[actorCount].posZ = ACTOR_LAMP2_POSZ;

                    actors[actorCount].posX = C2W(x + 0.5f);
                    actors[actorCount].posY = C2W(y + 0.5f);
                    
                    actorCount ++;
                }
            }
        }
        
    }
    
}

void updateActors()
{
    
}

void drawActors()
{
    for(int i = 0; i < actorCount; i++)
    {
        addSortedSprite(actors[i].sprite, actors[i].posX, actors[i].posY, actors[i].posZ, actors[i].height);
    }
}
