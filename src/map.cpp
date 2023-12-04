#include <map.hpp>
#include <player.hpp>
#include <screen.hpp>

float viewDistance;
float fov;


char wallSteps[WALL_STEPS + 1] = "WKMOoL|li-";
char groundSteps[GROUND_STEPS + 1] = "~-.";

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

float rayStep = 0.1f;

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
