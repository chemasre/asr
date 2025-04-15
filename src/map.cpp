#include <map.hpp>

// int map[MAP_HEIGHT][MAP_WIDTH] = {
                                     // { 0x0100, 0x0100, 0x0100, 0x0101, 0x0101, 0x0100, 0x0100, 0x0100 },
                                     // { 0x0100, 0x0000, 0x0100, 0x0000, 0x0000, 0x0100, 0x0000, 0x0100 },
                                     // { 0x0100, 0x0000, 0x0100, 0x0000, 0x0000, 0x0100, 0x0000, 0x0100 },
                                     // { 0x0100, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0100 },
                                     // { 0x0100, 0x0000, 0x0100, 0x0000, 0x0000, 0x0100, 0x0000, 0x0100 },
                                     // { 0x0100, 0x0100, 0x0101, 0x0000, 0x0000, 0x0101, 0x0100, 0x0100 },
                                     // { 0x0100, 0x0000, 0x0101, 0x0000, 0x0000, 0x0101, 0x0000, 0x0100 },
                                     // { 0x0100, 0x0000, 0x0100, 0x0000, 0x0000, 0x0100, 0x0000, 0x0100 },
                                     // { 0x0100, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0100 },
                                     // { 0x0100, 0x0100, 0x0100, 0x0000, 0x0000, 0x0100, 0x0100, 0x0100 },
                                     // { 0x0100, 0x0100, 0x0100, 0x0101, 0x0101, 0x0100, 0x0100, 0x0100 },
                                 // };
                                 
int maps[MAX_MAPS][MAP_HEIGHT][MAP_WIDTH];

void initMap()
{
    char fileName[MAX_PATH_LENGTH] = "";
    char filePath[MAX_PATH_LENGTH] = "";
    
    for(int i = 0; i < MAX_MAPS; i++)
    {
        sprintf(fileName, MAP_FILENAME_PATTERN, i);
        sprintf(filePath, "%s\\%s", MAP_DIRECTORY,fileName);

        FILE* mapFile = fopen(filePath, "rb");
        
        ASSERT(mapFile != 0, "Cannot open map")
        
        int w;
        int h;
        
        fread(&w, sizeof(int), 1, mapFile);
        fread(&h, sizeof(int), 1, mapFile);
        
        ASSERT(w == MAP_WIDTH && h == MAP_HEIGHT, "Map size doesn't match")
        
        for(int y = 0; y < h; y ++)
        {
            for(int x = 0; x < w; x ++)
            {
                fread(&maps[i][y][x], sizeof(int), 1, mapFile);    
            }
            
        }
        
        printf("Loaded map %s\n", filePath);
        
        fclose(mapFile);
    }
    
   
}


int getMapCell(int m, int x, int y, int boundaryDefault)
{
    if(x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) { return boundaryDefault; }
    else { return maps[m][y][x]; }
}

int rayCastStep(int m, float prevPosX, float prevPosY, float nextPosX, float nextPosY, float *normal, int *tex, float *u)
{
    int result;
    int prevMapX = (int)prevPosX;
    int prevMapY = (int)prevPosY;
    int nextMapX = (int)nextPosX;
    int nextMapY = (int)nextPosY;
    
    int previousCell = getMapCell(m, prevMapX, prevMapY, MAKE_MAP_CELL(MAP_CELL_TYPE_WALL, 0));
    int nextCell = getMapCell(m, nextMapX, nextMapY, MAKE_MAP_CELL(MAP_CELL_TYPE_WALL, 0));
    
    if(MAP_CELL_TYPE(previousCell) != MAP_CELL_TYPE_WALL && MAP_CELL_TYPE(nextCell) == MAP_CELL_TYPE_WALL)
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

        *tex = MAP_CELL_PARAM(nextCell);
    }
    
    if(MAP_CELL_TYPE(nextCell) == MAP_CELL_TYPE_WALL)
    {
        result = 1;
    }
    else
    {
        result = 0;
    }
    
    return result;
}

int rayCast(int m, float posX, float posY, float angle, float rayStep, float rayDistance, float *hitDistance, float *hitNormal, int *hitTex, float *hitV)
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
        
        if(rayCastStep(m, rayX, rayY, rayNextX, rayNextY, &normal, &tex, &v))
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

