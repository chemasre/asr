#include <ui.hpp>
#include <map.hpp>
#include <player.hpp>
#include <view.hpp>
#include <textures.hpp>

#define LIGHTSTEPS 9
#define GROUND_STEPS 3
#define SKY_STEPS 3

#define WALL_COLOR MAKE_COLOR(136, 68, 0)
#define GROUND_COLOR MAKE_COLOR(131, 5, 5)
#define SKY_COLOR MAKE_COLOR(149, 74, 0)


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


void drawColumn(int screenX, float normalizedDistance, float direction, int texture, float u)
{
    float dirX = cos(direction * DEG2RAD);
    float dirY = sin(direction * DEG2RAD);
    float sunLightDirX = cos(sunLightDirection * DEG2RAD); 
    float sunLightDirY = sin(sunLightDirection * DEG2RAD);

    float dot = dirX * sunLightDirX + dirY * sunLightDirY;    
    float light = clamp01(ambientLightIntensity + (dot > 0 ? sunLightIntensity * dot: 0));
    
    float halfFov = fov / 2;
    float distance = normalizedDistance * viewDistance;
    float hypotenuse = distance / cos(halfFov * DEG2RAD);
    float viewHeight = getViewHeight();
    float viewScale = viewHeight / screenHeight;
    
    // int h = screenHeight * (1 - normalizedDistance) * (1 - normalizedDistance);
    
    for(int screenY = 0; screenY < screenHeight; screenY ++)
    {
        float viewY = viewScale * -(screenY - screenHeight / 2.0f);
        float angle = atan2(viewY, VIEW_NEAR_DISTANCE);
        float viewWorldZ = VIEW_WORLD_Z + distance * sin(angle);
        
        if(viewWorldZ <= MAP_CELL_WORLD_SIZE && viewWorldZ >= 0)
        {
            float v = 0;
            TextureColor sample;
            TextureColor boundaryDefault;
            boundaryDefault.r = 0.3f;
            boundaryDefault.g = 0.3f;
            boundaryDefault.b = 0.3f;

            // if(viewWorldZ > 0)
            // {
            v = 1.0f - W2C(viewWorldZ);
            
            sample = getTextureSample(texture, u, v, boundaryDefault);
            // }
            
            float sampleLight = (sample.r + sample.g + sample.b) / 3.0f;
                
            int lightStep = (int)((clamp01(normalizedDistance + (1 - light) * (1 - sampleLight)) * LIGHTSTEPS));
            if(lightStep >= LIGHTSTEPS) { lightStep = LIGHTSTEPS - 1; }
            else if(lightStep < 0) { lightStep = 0; }
            
            char l = lightSteps[lightStep];
            
            int wallColor = MAKE_COLOR((int)(sample.r * 255), (int)(sample.g * 255), (int)(sample.b * 255));
            
            setScreenCell(screenX, screenY, wallColor, l);

            // if(screenY % 5 == 0 && screenX % 10 == 0)
            // {
                // drawFloat(screenX, screenY, "U", u);
                // drawFloat(screenX, screenY + 1, "V", v);
                // drawFloat(screenX, screenY + 2, "T", sample);
            // }
            
            // if(screenY % 5 == 0 && screenX % 10 == 0)
            // {
                // drawFloat(screenX, screenY, "VY", viewY);
                // drawFloat(screenX, screenY + 1, "A", angle * RAD2DEG);
                // drawFloat(screenX, screenY + 2, "WZ", viewWorldZ);
            // }
            
            // if(screenY % 5 == 0 && screenX % 10 == 0)
            // {
                // float viewHeight = getViewHeight();
                // float viewWidth = getViewHeight() * getViewAspect();
                
                // drawFloat(screenX, screenY, "VH", viewHeight);
                // drawFloat(screenX, screenY + 1, "VW", viewWidth);
                // drawFloat(screenX, screenY + 2, "VA", getViewAspect());
                // drawFloat(screenX, screenY + 3, "WZ", viewWorldZ);
            // }
        }
        else if(viewWorldZ < 0)  
        {
            // ground
            
            float groundDistanceFactor = 1 - (screenY - (screenHeight/2.0f)) / (screenHeight/2.0f);
            int groundStep = (int)(groundDistanceFactor * GROUND_STEPS);
            if(groundStep >= GROUND_STEPS) { groundStep = GROUND_STEPS - 1; }
        
            char g = groundSteps[groundStep];
            
            setScreenCell(screenX, screenY, GROUND_COLOR, g);
        }
        else
        {
            // sky

            float skyHeightFactor = 1 - ((screenHeight/2.0f) - screenY)  / (screenHeight/2.0f);
            int skyStep = (int)(skyHeightFactor * SKY_STEPS);            
            if(skyStep >= SKY_STEPS) { skyStep = SKY_STEPS - 1; }
            char s = skySteps[skyStep];            
            setScreenCell(screenX, screenY, SKY_COLOR, s);
        }
    }
}

float getViewHeight()
{
    float halfFov = fov / 2;    
    float h = VIEW_NEAR_DISTANCE / cos(halfFov * DEG2RAD);
    float vH = sin(halfFov * DEG2RAD) * h * 2;
    
    return vH;
}

float getViewAspect()
{
    float screenAspect = (screenWidth * fontWidth) / (float)(screenHeight * fontHeight);    
    
    return screenAspect;
}

void drawView()
{
    float centerX = (float)screenWidth / 2;
    
    float viewHeight = getViewHeight();
    float viewWidth = viewHeight * getViewAspect();
    
    float viewScale = viewWidth / screenWidth;
    
    for(int screenX = 0; screenX < screenWidth; screenX ++)
    {        
        float normalizedDistance;
        
        float distance;
        float normal;
        int texture;
        float u;
        
        float screenDeviationX = screenX - centerX;
        float viewDeviationX = screenDeviationX * viewScale;
        float viewDeviationAngle = atan2(viewDeviationX, VIEW_NEAR_DISTANCE) * RAD2DEG;

        int hit = rayCast(playerPosX, playerPosY, playerAngle + viewDeviationAngle, rayStep, viewDistance, &distance, &normal, &texture, &u);        

        if(!hit)
        {
            normalizedDistance = 1;
        }
        else
        {
            normalizedDistance = distance / viewDistance;
        }
        
       
        drawColumn(screenX, normalizedDistance, normal, texture, u);
        
        // if(screenX % 20 == 0)
        // {
            // drawFloat(screenX, screenHeight/2, "VHA", viewDeviationAngle);
        // }
        
    }
}


