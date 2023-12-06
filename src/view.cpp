#include <map.hpp>
#include <player.hpp>
#include <view.hpp>

#define FOV 30.0f
#define MAX_FOV 30.0f
#define MIN_FOV 10.0f
#define FOV_STEPS 10

#define VIEW_DISTANCE 5.0f
#define MIN_VIEW_DISTANCE 2.0f
#define MAX_VIEW_DISTANCE 10.0f
#define VIEW_DISTANCE_STEPS 10
#define SUNLIGHT_DIRECTION_STEPS 10
#define LIGHT_INTENSITY_STEPS 10

#define LIGHTSTEPS 12
#define GROUND_STEPS 3

float viewDistance;
float fov;


char lightSteps[LIGHTSTEPS + 1] = "WKMOyolic;:.";
char groundSteps[GROUND_STEPS + 1] = "+~-";

float rayStep = 0.1f;

float sunLightDirection = 45;
float sunLightIntensity = 1.0f;

float ambientLightIntensity = 0.6f;

void initView()
{
    fov = FOV;
    viewDistance = VIEW_DISTANCE;    
}

void updateView()
{
    int increaseFov = isKeyPressed(KEY_2);
    int decreaseFov = isKeyPressed(KEY_1);
    
    if(increaseFov || decreaseFov)
    {
        float fovStep = (MAX_FOV - MIN_FOV) / FOV_STEPS;
        
        if(increaseFov)
        {
            fov = fov + fovStep;
        }
        else
        {
            fov = fov - fovStep;
        }
        
        if(fov < MIN_FOV) { fov = MIN_FOV; }
        else if(fov > MAX_FOV) { fov = MAX_FOV; }
        
    }
    
    int increaseViewDistance = isKeyPressed(KEY_6);
    int decreaseViewDistance = isKeyPressed(KEY_5);
    
    if(increaseViewDistance || decreaseViewDistance)
    {
        float viewDistanceStep = (MAX_VIEW_DISTANCE - MIN_VIEW_DISTANCE) / VIEW_DISTANCE_STEPS;
        
        if(increaseViewDistance)
        {
            viewDistance = viewDistance + viewDistanceStep;
        }
        else
        {
            viewDistance = viewDistance - viewDistanceStep;
        }
        
        if(viewDistance < MIN_VIEW_DISTANCE) { viewDistance = MIN_VIEW_DISTANCE; }
        else if(viewDistance > MAX_VIEW_DISTANCE) { viewDistance = MAX_VIEW_DISTANCE; }
        
    }

    int increaseSunLightDirection = isKeyPressed(KEY_8);
    int decreaseSunLightDirection = isKeyPressed(KEY_7);
    
    if(increaseSunLightDirection || decreaseSunLightDirection)
    {
        float sunLightDirectionStep = 360.0f / SUNLIGHT_DIRECTION_STEPS;
        
        if(increaseSunLightDirection)
        {
            sunLightDirection = sunLightDirection + sunLightDirectionStep;            
        }
        else
        {
            sunLightDirection = sunLightDirection - sunLightDirectionStep;            
        }
        
        if(sunLightDirection < 0) { sunLightDirection += 360.0f; }
        else if(sunLightDirection > 360.0f) { sunLightDirection -= 360.0f; }
        
    }
    
    int increaseSunLightIntensity = isKeyPressed(KEY_0);
    int decreaseSunLightIntensity = isKeyPressed(KEY_9);
    
    if(increaseSunLightIntensity || decreaseSunLightIntensity)
    {
        float sunLightIntensityStep = 1.0f / LIGHT_INTENSITY_STEPS;
        
        if(increaseSunLightIntensity)
        {
            sunLightIntensity = sunLightIntensity + sunLightIntensityStep;            
        }
        else
        {
            sunLightIntensity = sunLightIntensity - sunLightIntensityStep;            
        }
        
        if(sunLightIntensity < 0) { sunLightIntensity = 0; }
        else if(sunLightIntensity > 1.0f) { sunLightIntensity = 1.0f; }
        
    }
    
    int increaseAmbientLightIntensity = isKeyPressed(KEY_4);
    int decreaseAmbientLightIntensity = isKeyPressed(KEY_3);
    
    if(increaseAmbientLightIntensity || decreaseAmbientLightIntensity)
    {
        float ambientLightIntensityStep = 1.0f / LIGHT_INTENSITY_STEPS;
        
        if(increaseAmbientLightIntensity)
        {
            ambientLightIntensity = ambientLightIntensity + ambientLightIntensityStep;            
        }
        else
        {
            ambientLightIntensity = ambientLightIntensity - ambientLightIntensityStep;            
        }
        
        if(ambientLightIntensity < 0) { ambientLightIntensity = 0; }
        else if(ambientLightIntensity > 1.0f) { ambientLightIntensity = 1.0f; }
        
    }    

}


void drawColumn(int x, float depth, float direction)
{
    float dirX = cos(direction * DEG2RAD);
    float dirY = sin(direction * DEG2RAD);
    float sunLightDirX = cos(sunLightDirection * DEG2RAD); 
    float sunLightDirY = sin(sunLightDirection * DEG2RAD);

    float dot = dirX * sunLightDirX + dirY * sunLightDirY;    
    float light = ambientLightIntensity + (dot > 0 ? sunLightIntensity * dot: 0);
    
    int h = screenHeight * (1 - depth);
    
    int lightStep = (int)((depth + (1 - light)) * LIGHTSTEPS);
    if(lightStep >= LIGHTSTEPS) { lightStep = LIGHTSTEPS - 1; }
    else if(lightStep < 0) { lightStep = 0; }
    
    char l = lightSteps[lightStep];
    
    for(int y = 0; y < screenHeight; y ++)
    {
        if(y <= screenHeight/2 + h/2 && y >= screenHeight/2 - h/2)
        {
            screen[y][x] = l; 
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
        float depth;
        
        float distance;
        float normal;

        int hit = rayCast(playerPosX, playerPosY, playerAngle + (x - centerX) / (float)screenWidth * fov / 2, rayStep, viewDistance, &distance, &normal);

        if(!hit)
        {
            depth = 1;
        }
        else
        {
            depth = distance / viewDistance;
        }
        
       
        drawColumn(x, depth, normal);
    }
}

int rayCastStep(float prevPosX, float prevPosY, float nextPosX, float nextPosY, float *normal)
{
    int result;
    int prevMapX = (int)prevPosX;
    int prevMapY = (int)prevPosY;
    int nextMapX = (int)nextPosX;
    int nextMapY = (int)nextPosY;
    
    int previousCell = getMapCell(prevMapX, prevMapY, MAP_CELL_WALL);
    int nextCell = getMapCell(nextMapX, nextMapY, MAP_CELL_WALL);
    
    if(previousCell == MAP_CELL_FREE && nextCell != MAP_CELL_FREE)
    {
        if(prevMapY < nextMapY)
        {
            *normal = 90;
        }
        else if(prevMapY > nextMapY)
        {
            *normal = 270;
        }
        else if(prevMapX < nextMapX)
        {
            *normal = 180;
        }
        else // prevMapX > nextMapX
        {
            *normal = 0;
        }
    }
    
    if(nextCell != MAP_CELL_FREE)
    {
        result = 1;
    }
    else
    {
        result = 0;
    }
    
    return result;
}

int rayCast(float posX, float posY, float angle, float rayStep, float rayDistance, float *hitDistance, float *hitNormal)
{
    // Assumes that there's no map collision at posX and posY 
    
    float distance = 0;
    float normal;
    int collision = 0;
            
    float sinAngle = sin(angle * DEG2RAD);
    float cosAngle = cos(angle * DEG2RAD);
    
    while(distance <= rayDistance && !collision)
    {
        float rayEndX = posX + cosAngle * distance;
        float rayEndY = posY + sinAngle * distance;
        
        if(rayCastStep(posX, posY, rayEndX, rayEndY, &normal))
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
        
        *hitDistance = distance;
        *hitNormal = normal;

        return 1;
    }
    else
    {
        return 0;
    }
}
