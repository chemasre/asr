#include <ui.hpp>
#include <map.hpp>
#include <player.hpp>
#include <view.hpp>
#include <textures.hpp>

#define LIGHTSTEPS 9
#define GROUND_STEPS 3
#define SKY_STEPS 3


float viewDistance;
float fov;


char lightSteps[LIGHTSTEPS + 1] = "@&8OCc;:.";
char groundSteps[GROUND_STEPS + 1] = "+~-";
char skySteps[SKY_STEPS + 1] = "=\"'";

float rayStep = 0.1f;

float sunLightDirection = 45;
float sunLightIntensity = 0.5f;

float ambientLightIntensity = 0.5f;


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


void drawColumn(int x, float depth, float direction, float u)
{
    float dirX = cos(direction * DEG2RAD);
    float dirY = sin(direction * DEG2RAD);
    float sunLightDirX = cos(sunLightDirection * DEG2RAD); 
    float sunLightDirY = sin(sunLightDirection * DEG2RAD);

    float dot = dirX * sunLightDirX + dirY * sunLightDirY;    
    float light = clamp01(ambientLightIntensity + (dot > 0 ? sunLightIntensity * dot: 0));
    
    int h = screenHeight * (1 - depth) * (1 - depth);
    
    for(int y = 0; y < screenHeight; y ++)
    {
        if(y <= screenHeight/2 + h/2 && y >= screenHeight/2 - h/2)
        {
            float v = 0;
            float texture = 0.3f;

            if(h > 0)
            {
                float halfScreen = (float)screenHeight/2.0f;
                float halfHeight = (float)h/2.0f;
                float height = (float)h;
                
                v = (y - (halfScreen - halfHeight)) / height;
                texture = getTextureSample(0, u, v, 0.3f);
                
            }
                
            int lightStep = (int)((clamp01(depth + (1 - light) * (1 - texture)) * LIGHTSTEPS));
            if(lightStep >= LIGHTSTEPS) { lightStep = LIGHTSTEPS - 1; }
            else if(lightStep < 0) { lightStep = 0; }
            
            char l = lightSteps[lightStep];
            
            screen[y][x] = l;
            
            // if(y % 5 == 0 && x % 10 == 0)
            // {
                // drawFloat(x, y, "U", u);
                // drawFloat(x, y + 1, "V", v);
                // drawFloat(x, y + 2, "T", texture);
            // }
            
        }
        else if(y > screenHeight/2 + h/2)  
        {
            // ground
            
            float groundDistanceFactor = 1 - (y - (screenHeight/2.0f)) / (screenHeight/2.0f);
            int groundStep = (int)(groundDistanceFactor * GROUND_STEPS);
            if(groundStep >= GROUND_STEPS) { groundStep = GROUND_STEPS - 1; }
        
            char g = groundSteps[groundStep];
            
            screen[y][x] = g;
        }
        else
        {
            // sky

            float skyHeightFactor = 1 - ((screenHeight/2.0f) - y)  / (screenHeight/2.0f);
            int skyStep = (int)(skyHeightFactor * SKY_STEPS);            
            if(skyStep >= SKY_STEPS) { skyStep = SKY_STEPS - 1; }
            char s = skySteps[skyStep];            
            screen[y][x] = s; 
        }
    }
}

void drawView()
{
    float centerX = (float)screenWidth / 2;
    float screenAspect = (float)(screenHeight * FONT_HEIGHT) / (screenWidth * FONT_WIDTH);
    
    float halfFov = fov / 2;
    
    float h = VIEW_NEAR_DISTANCE / cos(halfFov * DEG2RAD);
    float viewHeight = sin(halfFov * DEG2RAD) * h;
    float viewWidth = viewHeight * screenAspect;
    float viewScale = viewWidth / screenWidth;
    
    for(int x = 0; x < screenWidth; x ++)
    {        
        float depth;
        
        float distance;
        float normal;
        float u;
        
        float screenDeviationX = x - centerX;
        float viewDeviationX = screenDeviationX * viewScale;
        float viewDeviationAngle = atan2(viewDeviationX, VIEW_NEAR_DISTANCE) * RAD2DEG;

        int hit = rayCast(playerPosX, playerPosY, playerAngle + viewDeviationAngle, rayStep, viewDistance, &distance, &normal, &u);        

        if(!hit)
        {
            depth = 1;
        }
        else
        {
            depth = distance / viewDistance;
        }
        
       
        drawColumn(x, depth, normal, u);
    }
}

int rayCastStep(float prevPosX, float prevPosY, float nextPosX, float nextPosY, float *normal, float *u)
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

int rayCast(float posX, float posY, float angle, float rayStep, float rayDistance, float *hitDistance, float *hitNormal, float *hitV)
{
    // Assumes that there's no map collision at posX and posY 
    
    float normal;
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
        
        if(rayCastStep(rayX, rayY, rayNextX, rayNextY, &normal, &v))
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
        
        *hitDistance = sqrt(distance2);
        *hitNormal = normal;
        *hitV = v;

        return 1;
    }
    else
    {
        return 0;
    }
}
