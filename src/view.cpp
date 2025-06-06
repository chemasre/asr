#include <ui.hpp>
#include <map.hpp>
#include <player.hpp>
#include <view.hpp>
#include <sprites.hpp>
#include <textures.hpp>

#define LIGHTSTEPS 9
#define GROUND_STEPS 3
#define SKY_STEPS 3

#define WALL_COLOR MAKE_COLOR(136, 68, 0)
#define GROUND_COLOR MAKE_COLOR(131, 5, 5)
#define SKY_COLOR MAKE_COLOR(149, 74, 0)

int viewportWidth;
int viewportHeight;

int viewportCropBottom;
int viewportCropRight;

float viewDistance;
float fov;

char lightSteps[LIGHTSTEPS + 1] = "@&8OCc;:.";
char groundSteps[GROUND_STEPS + 1] = "+~-";
char skySteps[SKY_STEPS + 1] = "=\"'";

float rayStep = 0.1f;

float sunLightDirection = 45;
float sunLightIntensity = 0.5f;

float ambientLightIntensity = 0.5f;

float* zBuffer;
int zBufferWidth;


struct SortedSprite
{
    float posX;
    float posY;
    float posZ;
    int sprite;
    float height;
};

SortedSprite sortedSprites[MAX_SORTED_SPRITES];
float sortedSpritesDistances[MAX_SORTED_SPRITES];
int sortedSpritesCount;

void setZBufferWidth(int w)
{    
    if(zBuffer != 0)
    {   delete zBuffer; 
        zBuffer = 0;
    }    
    
    zBufferWidth = w;
    
    zBuffer = new float[zBufferWidth];
    for(int i = 0; i < zBufferWidth; i ++) { zBuffer[i] = 0; }
    
}

void clearZBuffer()
{
    for(int i = 0; i < zBufferWidth; i++) { zBuffer[i] = 0; }
}

void initZBuffer()
{
    zBuffer = 0;  
    setZBufferWidth(viewportWidth);
}

void setZBufferValue(int x, float v)
{
    if(x < 0 || x > zBufferWidth - 1) { return; }
    zBuffer[x] = v;
}

float getZBufferValue(int x)
{
    if(x < 0 || x > zBufferWidth - 1) { return 0; }
    else { return zBuffer[x]; }
}

void initSortedSprites()
{
    sortedSpritesCount = 0;
}

void addSortedSprite(int sprite, float x, float y, float z, float height)
{
    sortedSprites[sortedSpritesCount].posX = x;
    sortedSprites[sortedSpritesCount].posY = y;
    sortedSprites[sortedSpritesCount].posZ = z;
    sortedSprites[sortedSpritesCount].height = height;
    sortedSprites[sortedSpritesCount].sprite = sprite;
    
    sortedSpritesCount ++;
}

void clearSortedSprites()
{
    sortedSpritesCount = 0;
}

void drawSortedSprites()
{
    // Sort sprite list by distance to player
    
    for(int i = 0; i < sortedSpritesCount; i++)
    {
        float aX = cameraPosX - sortedSprites[i].posX;
        float aY = cameraPosY - sortedSprites[i].posY;
        sortedSpritesDistances[i] = sqrt(aX * aX + aY * aY);
    }
    
    for(int i = 0; i < sortedSpritesCount; i ++)
    {
        for(int j = i + 1; j < sortedSpritesCount; j++)
        {
            if(sortedSpritesDistances[j] > sortedSpritesDistances[i])
            {
                float d = sortedSpritesDistances[j];
                sortedSpritesDistances[j] = sortedSpritesDistances[i];
                sortedSpritesDistances[i] = d;
 
                SortedSprite s = sortedSprites[j];
                sortedSprites[j] = sortedSprites[i];
                sortedSprites[i] = s;
            }                
        }
    }
    
    // Draw sprites
    
    for(int i = 0; i < sortedSpritesCount; i++)
    {
        float spritePosX = sortedSprites[i].posX;
        float spritePosY = sortedSprites[i].posY;
        float spritePosZ = sortedSprites[i].posZ;
        
        float playerToSpriteX  = spritePosX - cameraPosX;
        float playerToSpriteY  = spritePosY - cameraPosY;
        
        float spriteAngle = atan2(playerToSpriteY, playerToSpriteX) * RAD2DEG;
        
        float playerToSpriteAngle = spriteAngle - (playerAngle - 90);
        
        float spritePlayerX = cos(playerToSpriteAngle * DEG2RAD) * sortedSpritesDistances[i];
        float spritePlayerY = sin(playerToSpriteAngle * DEG2RAD) * sortedSpritesDistances[i];
        float spriteLowPlayerZ = -VIEW_WORLD_Z + spritePosZ;
        float spriteHighPlayerZ = -VIEW_WORLD_Z + spritePosZ + sortedSprites[i].height;

        float viewHeight = getViewHeight();
        float viewWidth = viewHeight * getViewAspect();
        
        float h = VIEW_NEAR_DISTANCE / sin(playerToSpriteAngle * DEG2RAD);
        float spriteViewX = - h * cos(playerToSpriteAngle * DEG2RAD);
        
        int spriteViewportX = viewportWidth / 2 + spriteViewX * viewportWidth / viewWidth;
        
        float spriteVerticalLowAngle = atan2(spriteLowPlayerZ, sortedSpritesDistances[i]) * RAD2DEG;
        float spriteVerticalHighAngle = atan2(spriteHighPlayerZ, sortedSpritesDistances[i]) * RAD2DEG;
        
        h = VIEW_NEAR_DISTANCE / cos(spriteVerticalLowAngle * DEG2RAD);
        float spriteViewLowZ = -h * sin(spriteVerticalLowAngle * DEG2RAD);

        int spriteViewportLowY = viewportHeight / 2 + spriteViewLowZ * viewportHeight / viewHeight;
        
        h = VIEW_NEAR_DISTANCE / cos(spriteVerticalHighAngle * DEG2RAD);
        float spriteViewHighZ = -h * sin(spriteVerticalHighAngle * DEG2RAD);

        int spriteViewportHighY = viewportHeight / 2 + spriteViewHighZ * viewportHeight / viewHeight;
        
        

        if(spritePlayerY > 0)
        {
            // Search for a visible sprite position
            
            int clipStartX = -1;
            float normalizedDistance = sortedSpritesDistances[i] / VIEW_DISTANCE;
            int spriteLeft = spriteViewportX - sprites[sortedSprites[i].sprite].pivotX;

            for(int j = spriteLeft; j < spriteLeft + SPRITE_SIZE; j++)
            {
                if(normalizedDistance < getZBufferValue(j)) { clipStartX = j; }
            }
            
            if(clipStartX >= 0)
            {
                // Swipe left looking for the first occluded position
                
                int clipLeft = clipStartX;
                while(normalizedDistance < getZBufferValue(clipLeft) && clipLeft > 0) { clipLeft --; }
                
                // Swipe right looking for the first occluded position
                
                int clipRight = clipStartX;
                while(normalizedDistance < getZBufferValue(clipRight) && clipRight < viewportWidth - 1) { clipRight ++; }

                if(clipRight - clipLeft > 0)
                {
                    // Clip the sprite using the visible rect found
                    
                    float spriteScale = fabs(spriteViewportLowY - spriteViewportHighY) / SPRITE_SIZE;
                    
                    enableClipArea();
                    setClipArea(clipLeft, 0, clipRight - clipLeft, viewportHeight);            
                    //drawSprite(sortedSprites[i].sprite, spriteViewportX, spriteViewportY);
                    drawScaledSprite(sortedSprites[i].sprite, spriteViewportX, spriteViewportLowY, spriteScale);
                    disableClipArea();
                }
            }
            
        }
        
    }
}

void setViewportCrop(int right, int bottom)
{
    viewportCropBottom = bottom;
    viewportCropRight = right;
}


void initView()
{
    fov = FOV;
    viewDistance = VIEW_DISTANCE;
    
    viewportWidth = screenWidth;
    viewportHeight = screenHeight;
    
    initZBuffer();
    initSortedSprites();
}

void updateView()
{
    viewportWidth = screenWidth - viewportCropRight;
    viewportHeight = screenHeight - viewportCropBottom;    
    
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
    
    if(zBufferWidth != viewportWidth) { setZBufferWidth(viewportWidth); }

}


void drawColumn(int viewportX, float normalizedDistance, float direction, int texture, float u)
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
    float viewScale = viewHeight / viewportHeight;
    
    float viewX = viewScale * (viewportX - viewportWidth / 2.0f) / getViewAspect();
    float angleX = atan2(viewX, VIEW_NEAR_DISTANCE);
    
    
    // int h = viewportHeight * (1 - normalizedDistance) * (1 - normalizedDistance);
    
    for(int viewportY = 0; viewportY < viewportHeight; viewportY ++)
    {
        float viewY = viewScale * -(viewportY - viewportHeight / 2.0f);
        float angleY = atan2(viewY, VIEW_NEAR_DISTANCE);
        float viewWorldZ = VIEW_WORLD_Z + distance * sin(angleY);
        
        if(viewWorldZ <= MAP_CELL_WORLD_SIZE && viewWorldZ >= 0)
        {
            float v = 0;
            TextureColor sample;
            TextureColor boundaryDefault;
            boundaryDefault.r = 0.3f;
            boundaryDefault.g = 0.3f;
            boundaryDefault.b = 0.3f;

            v = 1.0f - W2C(viewWorldZ);
            
            sample = getTextureSample(texture, u, v, boundaryDefault);
            
            float sampleLight = (sample.r + sample.g + sample.b) / 3.0f;
                
            int lightStep = (int)((clamp01(normalizedDistance + (1 - light) * (1 - sampleLight)) * LIGHTSTEPS));
            if(lightStep >= LIGHTSTEPS) { lightStep = LIGHTSTEPS - 1; }
            else if(lightStep < 0) { lightStep = 0; }
            
            char l = lightSteps[lightStep];
            
            int wallColor = MAKE_COLOR((int)(sample.r * 255), (int)(sample.g * 255), (int)(sample.b * 255));
            
            setScreenCell(viewportX, viewportY, wallColor, l);
            
            setZBufferValue(viewportX, normalizedDistance);

            // if(viewportY % 5 == 0 && viewportX % 10 == 0)
            // {
                // drawFloat(viewportX, viewportY, "U", u);
                // drawFloat(viewportX, viewportY + 1, "V", v);
                // drawFloat(viewportX, viewportY + 2, "T", sample);
            // }
            
            // if(viewportY % 5 == 0 && viewportX % 10 == 0)
            // {
                // drawFloat(viewportX, viewportY, "VY", viewY);
                // drawFloat(viewportX, viewportY + 1, "A", angleY * RAD2DEG);
                // drawFloat(viewportX, viewportY + 2, "WZ", viewWorldZ);
            // }
            
            // if(viewportY % 5 == 0 && viewportX % 10 == 0)
            // {
                // float viewHeight = getViewHeight();
                // float viewWidth = getViewHeight() * getViewAspect();
                
                // drawFloat(viewportX, viewportY, "VH", viewHeight);
                // drawFloat(viewportX, viewportY + 1, "VW", viewWidth);
                // drawFloat(viewportX, viewportY + 2, "VA", getViewAspect());
                // drawFloat(viewportX, viewportY + 3, "WZ", viewWorldZ);
            // }
        }
        else if(viewWorldZ < 0)  
        {
            float groundDistance = fabs(- VIEW_WORLD_Z / sin(angleY));
            float groundDistanceXY = fabs(cos(angleY) * groundDistance);
            float groundPosX = cameraPosX + cos(playerAngle * DEG2RAD + angleX) * groundDistanceXY;
            float groundPosY = cameraPosY + sin(playerAngle * DEG2RAD + angleX) * groundDistanceXY;
            
            float tU = groundPosX / MAP_CELL_WORLD_SIZE;
            float groundU = tU - (int)tU;

            float tV = groundPosY / MAP_CELL_WORLD_SIZE;
            float groundV = tV - (int)tV;
            
            TextureColor boundaryDefault;
            boundaryDefault.r = 0.3f;
            boundaryDefault.g = 0.3f;
            boundaryDefault.b = 0.3f;
            
            TextureColor sample = getTextureSample(3, groundU, groundV, boundaryDefault);
            //int groundColor = MAKE_COLOR((int)(groundU * 255), (int)(groundV * 255), 0);
            int groundColor = MAKE_COLOR((int)(sample.r * 255), (int)(sample.g * 255), (int)(sample.b * 255));
                       
            
            // ground
            
            float groundDistanceFactor = 1 - (viewportY - (viewportHeight/2.0f)) / (viewportHeight/2.0f);
            int groundStep = (int)(groundDistanceFactor * GROUND_STEPS);
            if(groundStep >= GROUND_STEPS) { groundStep = GROUND_STEPS - 1; }
        
            char g = groundSteps[groundStep];
            
            // float sampleLight = (sample.r + sample.g + sample.b) / 3.0f;
                
            // int lightStep = (int)((clamp01((1 - ambientLightIntensity) * (1 - sampleLight)) * GROUND_STEPS));
            // if(lightStep >= GROUND_STEPS) { lightStep = GROUND_STEPS - 1; }
            // else if(lightStep < 0) { lightStep = 0; }
            
            // g = groundSteps[lightStep];            
            
            setScreenCell(viewportX, viewportY, groundColor, g);
            
             // if(viewportX % 14 == 0)
            // {
                // char text[100];
                // sprintf(text, "NGP %.2f %.2f", groundPosX / MAP_CELL_WORLD_SIZE, groundPosY / MAP_CELL_WORLD_SIZE);
                // drawString(MAKE_COLOR(255,0,0), text, viewportX, viewportY);
            // }
        }
        else
        {
            float skyDistance = fabs((- VIEW_WORLD_Z + MAP_CELL_WORLD_SIZE) / sin(angleY));
            float skyDistanceXY = fabs(cos(angleY) * skyDistance);
            float skyPosX = cameraPosX + cos(playerAngle * DEG2RAD + angleX) * skyDistanceXY;
            float skyPosY = cameraPosY + sin(playerAngle * DEG2RAD + angleX) * skyDistanceXY;
            
            float tU = skyPosX / MAP_CELL_WORLD_SIZE;
            float skyU = tU - (int)tU;

            float tV = skyPosY / MAP_CELL_WORLD_SIZE;
            float skyV = tV - (int)tV;
            
            TextureColor boundaryDefault;
            boundaryDefault.r = 0.3f;
            boundaryDefault.g = 0.3f;
            boundaryDefault.b = 0.3f;
            
            TextureColor sample = getTextureSample(4, skyU, skyV, boundaryDefault);
            //int groundColor = MAKE_COLOR((int)(groundU * 255), (int)(groundV * 255), 0);
            int skyColor = MAKE_COLOR((int)(sample.r * 255), (int)(sample.g * 255), (int)(sample.b * 255));


            // sky

            float skyHeightFactor = 1 - ((viewportHeight/2.0f) - viewportY)  / (viewportHeight/2.0f);
            int skyStep = (int)(skyHeightFactor * SKY_STEPS);            
            if(skyStep >= SKY_STEPS) { skyStep = SKY_STEPS - 1; }
            char s = skySteps[skyStep];  
            
            setScreenCell(viewportX, viewportY, skyColor, s);
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
    float viewportAspect = (viewportWidth * fontWidth) / (float)(viewportHeight * fontHeight);    
    
    return viewportAspect;
}

void drawView()
{
    clearZBuffer();
    
    float centerX = (float)viewportWidth / 2;
    
    float viewHeight = getViewHeight();
    float viewWidth = viewHeight * getViewAspect();
    
    float viewScale = viewWidth / viewportWidth;
    
    for(int viewportX = 0; viewportX < viewportWidth; viewportX ++)
    {        
        float normalizedDistance;
        
        float distance;
        float normal;
        int texture;
        float u;
        
        float viewportDeviationX = viewportX - centerX;
        float viewDeviationX = viewportDeviationX * viewScale;
        float viewDeviationAngle = atan2(viewDeviationX, VIEW_NEAR_DISTANCE) * RAD2DEG;

        int hit = rayCast(0, cameraPosX, cameraPosY, playerAngle + viewDeviationAngle, rayStep, viewDistance, &distance, &normal, &texture, &u);



        if(!hit)
        {
            normalizedDistance = 1;
            
            // Not well defined behaviour
            normal = 0;
            texture = 0;
            u = 0;
        }
        else
        {
            normalizedDistance = distance / viewDistance;
        }
        
        drawColumn(viewportX, normalizedDistance, normal, texture, u);
        
        // if(viewportX % 20 == 0)
        // {
            // drawFloat(viewportX, viewportHeight/2, "VHA", viewDeviationAngle);
        // }
        
    }
    
    drawSortedSprites();
    clearSortedSprites();
}


