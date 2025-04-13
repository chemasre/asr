#include <map.hpp>

int map[MAP_HEIGHT][MAP_WIDTH] = {
                                     { 100, 100, 100, 101, 101, 100, 100, 100 },
                                     { 100, 000, 100, 000, 000, 100, 000, 100 },
                                     { 100, 000, 100, 000, 000, 100, 000, 100 },
                                     { 100, 000, 000, 000, 000, 000, 000, 100 },
                                     { 100, 000, 100, 000, 000, 100, 000, 100 },
                                     { 100, 100, 101, 000, 000, 101, 100, 100 },
                                     { 100, 000, 101, 000, 000, 101, 000, 100 },
                                     { 100, 000, 100, 000, 000, 100, 000, 100 },
                                     { 100, 000, 000, 000, 000, 000, 000, 100 },
                                     { 100, 100, 100, 000, 000, 100, 100, 100 },
                                     { 100, 100, 100, 101, 101, 100, 100, 100 },
                                 };


int getMapCell(int x, int y, int boundaryDefault)
{
    if(x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) { return boundaryDefault; }
    else { return map[y][x]; }
}

int rayCastStep(float prevPosX, float prevPosY, float nextPosX, float nextPosY, float *normal, int *tex, float *u)
{
    int result;
    int prevMapX = (int)prevPosX;
    int prevMapY = (int)prevPosY;
    int nextMapX = (int)nextPosX;
    int nextMapY = (int)nextPosY;
    
    int previousCell = getMapCell(prevMapX, prevMapY, MAKE_CELL(MAP_CELL_TYPE_WALL, 0));
    int nextCell = getMapCell(nextMapX, nextMapY, MAKE_CELL(MAP_CELL_TYPE_WALL, 0));
    
    if(CTYPE(previousCell) == MAP_CELL_TYPE_FREE && CTYPE(nextCell) != MAP_CELL_TYPE_FREE)
    {
        if(prevMapY < nextMapY)
        {
            *normal = 90;
            *u = 1 - fabs(nextPosX - (int)nextPosX);
        }
        else if(prevMapY > nextMapY)
        {
            *normal = 270;
            *u = fabs(nextPosX - (int)nextPosX);
        }
        else if(prevMapX < nextMapX)
        {
            *normal = 180;
            *u = fabs(nextPosY - (int)nextPosY);
        }
        else // prevMapX > nextMapX
        {
            *normal = 0;
            *u = 1 - fabs(nextPosY - (int)nextPosY);
        }

        *tex = CTEX(nextCell);
    }
    
    if(CTYPE(nextCell) != MAP_CELL_TYPE_FREE)
    {
        result = 1;
    }
    else
    {
        result = 0;
    }
    
    return result;
}

int rayCast(float posX, float posY, float angle, float rayStep, float rayDistance, float *hitDistance, float *hitNormal, int *hitTex, float *hitV)
{
    posX = W2C(posX);
    posY = W2C(posY);
    rayStep = W2C(rayStep);
    rayDistance = W2C(rayDistance);
    
    // Assumes that there's no map collision at posX and posY 
    
    float normal;
    int tex;
    float v;
    int collision = 0;
            
    
    float stepX = cos(angle * DEG2RAD) * rayStep;
    float stepY = sin(angle * DEG2RAD) * rayStep;
    
    float rayDistance2 = rayDistance * rayDistance;
    float distance2 = 0;
    
    float rayX = posX;
    float rayY = posY;
   
    while(distance2 < rayDistance2 && !collision)
    {
        float distance = 0;
        
        float rayNextX = rayX + stepX;
        float rayNextY = rayY + stepY;
        
        float dx = (rayNextX - posX);
        float dy = (rayNextY - posY);
        distance2 =  dx * dx + dy * dy;
        
        if(rayCastStep(rayX, rayY, rayNextX, rayNextY, &normal, &tex, &v))
        {
            collision = 1;
        }
        else
        {
            rayX = rayNextX;
            rayY = rayNextY;
        }
    }
    
    if(collision)
    {
        if(distance2 > rayDistance2)
        {
            distance2 = rayDistance2;
        }
        
        *hitDistance = C2W(sqrt(distance2));
        *hitNormal = normal;
        *hitTex = tex;
        *hitV = v;

        return 1;
    }
    else
    {
        return 0;
    }
}

