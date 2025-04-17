#include <common_header.hpp>
#include <input.hpp>
#include <screen.hpp>
#include <ui.hpp>
#include <system.hpp>
#include <hud.hpp>
#include <info_line.hpp>
#include <player.hpp>
#include <map.hpp>
#include <sprites.hpp>
#include <textures.hpp>
#include <view.hpp>
#include <menu.hpp>

/**************************************************************
                   
                   A SIMPLE RAYCASTER
   
**************************************************************/

#define GAMESTATE_MAINMENU 0
#define GAMESTATE_PLAYING 1


#define MENU_MAIN 0
#define MENU_CONTROLS 1
#define MENU_GRAPHICS 2

#define MENUMAIN_START 0
#define MENUMAIN_DESCRIPTION_START (MENUMAIN_START + 1)
#define MENUMAIN_DESCRIPTION_SIZE 3
#define MENUMAIN_OPTIONS_START (MENUMAIN_DESCRIPTION_START + MENUMAIN_DESCRIPTION_SIZE)

#define MENUMAIN_OPTION_PLAY 0
#define MENUMAIN_OPTION_CONTROLS 1
#define MENUMAIN_OPTION_GRAPHICS 2
#define MENUMAIN_OPTION_EXIT 3

#define MENUMAIN_OPTIONS_SIZE 4

#define MENUMAIN_SIZE (1 + MENUMAIN_DESCRIPTION_SIZE + MENUMAIN_OPTIONS_SIZE)

#define MENUCONTROLS_START (MENUMAIN_START + MENUMAIN_SIZE)
#define MENUCONTROLS_DESCRIPTION_START (MENUCONTROLS_START + 1)
#define MENUCONTROLS_DESCRIPTION_SIZE 3
#define MENUCONTROLS_OPTIONS_START (MENUCONTROLS_DESCRIPTION_START + MENUCONTROLS_DESCRIPTION_SIZE)

#define MENUCONTROLS_OPTION_MODE 0
#define MENUCONTROLS_OPTION_MOUSE_SENSITIVITY 1
#define MENUCONTROLS_OPTION_BACK 2

#define MENUCONTROLS_OPTIONS_SIZE 3

#define MENUCONTROLS_SIZE (1 + MENUCONTROLS_DESCRIPTION_SIZE + MENUCONTROLS_OPTIONS_SIZE)

#define MENUGRAPHICS_START (MENUCONTROLS_START + MENUCONTROLS_SIZE)
#define MENUGRAPHICS_DESCRIPTION_START (MENUGRAPHICS_START + 1)
#define MENUGRAPHICS_DESCRIPTION_SIZE 0
#define MENUGRAPHICS_OPTIONS_START (MENUGRAPHICS_DESCRIPTION_START + MENUGRAPHICS_DESCRIPTION_SIZE)

#define MENUGRAPHICS_OPTION_LIGHT_AMBIENT_INTENSITY 0
#define MENUGRAPHICS_OPTION_LIGHT_DIRECTIONAL_INTENSITY 1
#define MENUGRAPHICS_OPTION_LIGHT_DIRECTIONAL_DIRECTION 2
#define MENUGRAPHICS_OPTION_BACK 3

#define MENUGRAPHICS_OPTIONS_SIZE 4

#define MENUGRAPHICS_SIZE (1 + MENUGRAPHICS_DESCRIPTION_SIZE + MENUGRAPHICS_OPTIONS_SIZE)

#define SCREEN_TITLE "A Simple Raycaster"

#define BACKGROUND_COLOR MAKE_COLOR(0, 0, 0)

#define MOUSE_SENSITIVITY_STEPS 10

#define MAP_PARAM_LIGHT_AMBIENT_INTENSITY 0
#define MAP_PARAM_LIGHT_DIRECTIONAL_DIRECTION 1
#define MAP_PARAM_LIGHT_DIRECTIONAL_INTENSITY 2

void main()
{
        
    initInput();
    initSprites();
    initTextures();
    initScreen();
    initUI();
    initHud();
    initInfoLine();
    initMap();
    initView();
    initPlayer();
    initMenu();
    
    setScreenTitle(SCREEN_TITLE);
    setClearColor(BACKGROUND_COLOR);
   
    sprintf(menuLines[MENUGRAPHICS_OPTIONS_START + 0], "Ambient intensity: %.2f", ambientLightIntensity);
    sprintf(menuLines[MENUGRAPHICS_OPTIONS_START + 1], "Sun intensity: %.2f", sunLightIntensity);
    sprintf(menuLines[MENUGRAPHICS_OPTIONS_START + 2], "Sun direction: %.2f", sunLightDirection);
    sprintf(menuLines[MENUCONTROLS_OPTIONS_START + MENUCONTROLS_OPTION_MODE], "Mode: %s", playerControlMode == PLAYER_CONTROL_MODE_CLASSIC ? "Classic" : "Modern");    
    sprintf(menuLines[MENUCONTROLS_OPTIONS_START + MENUCONTROLS_OPTION_MOUSE_SENSITIVITY], "Mouse sensitivity: %0.1f", mouseSensitivity);
    
    int isClassic = (playerControlMode == PLAYER_CONTROL_MODE_CLASSIC);
    const char* desc1 = isClassic ? "Move/Rotate: WASD" : "Move/Strafe: WASD";
    const char* desc2 = isClassic ? "Strafe: ALT + WASD": "Rotate: Mouse";
    
    sprintf(menuLines[MENUMAIN_DESCRIPTION_START + 0], desc1);
    sprintf(menuLines[MENUMAIN_DESCRIPTION_START + 1], desc2);

    sprintf(menuLines[MENUCONTROLS_DESCRIPTION_START + 0], desc1);
    sprintf(menuLines[MENUCONTROLS_DESCRIPTION_START + 1], desc2);
    
    int gameState = GAMESTATE_MAINMENU;
    int exit = 0;
    
    setMenu(MENU_MAIN, MENUMAIN_START, MENUMAIN_DESCRIPTION_START, MENUMAIN_DESCRIPTION_SIZE, MENUMAIN_OPTIONS_SIZE, 0);
    
    appendInfoLine("ONCE UPON A TIME DURING A HOLY WEEK CAME THIS LITTLE EXPERIMENT BY JOSE MANUEL (CHEMA) SOLIS");
    appendInfoLine("                            ");
    appendInfoLine("IT'S DEDICATED TO MY COUSIN, FRANCISCO MIGUEL, WHO LET ME PLAY WOLFENSTEIN 3D FOR THE FIRST TIME AT HIS PC, DURING THE EARLY 90s");
    appendInfoLine("                            ");
    appendInfoLine("IF YOU LIKE THIS KIND OF STUFF, YOU CAN FOLLOW ME ON HTTPS://MASTODON.SOCIAL/@CHEMAUNCH, WHERE I SHARE MY PROJECTS (IN SPANISH)");
    appendInfoLine("                            ");
    appendInfoLine("IF YOU DIDN'T CATCH THAT ADDRESS, TAKE A SCREENSHOT");
    appendInfoLine("                            ");    
    appendInfoLine("YOU ARE WELCOME");
    appendInfoLine("                            ");    
    appendInfoLine("BY THE WAY, NO AI HAS BEEN USED TO GENERATE THIS CODE");
    appendInfoLine("                            ");
    appendInfoLine("BUT YOU CAN TRAIN YOURS WITH IT");
    appendInfoLine("                            ");
    appendInfoLine("IF YOU ARE BRAVE");
    appendInfoLine("                            ");
    
    setInfoLineScrollPosition(screenWidth / 2);
    
    ambientLightIntensity = getMapParam(0, MAP_PARAM_LIGHT_AMBIENT_INTENSITY) * 1.0f / 15;
    sunLightDirection = getMapParam(0, MAP_PARAM_LIGHT_DIRECTIONAL_DIRECTION) * 360.0f / 16;
    sunLightIntensity = getMapParam(0, MAP_PARAM_LIGHT_DIRECTIONAL_INTENSITY) * 1.0f / 15;
    
    while(!exit)
    {
        updateInput();
        
        if(gameState == GAMESTATE_MAINMENU)
        {
            updateMenu();
            updateInfoLine();
            
            if(menu == MENU_MAIN)
            {
                if(isKeyDown(KEY_ESC))
                {
                    exit = 1;
                }
                else if(menuOptionSelected)
                {
                    if(menuOption == MENUMAIN_OPTION_PLAY)
                    {
                        gameState = GAMESTATE_PLAYING;
                    }
                    else if(menuOption == MENUMAIN_OPTION_CONTROLS)
                    {
                        setMenu(MENU_CONTROLS, MENUCONTROLS_START, MENUCONTROLS_DESCRIPTION_START, MENUCONTROLS_DESCRIPTION_SIZE, MENUCONTROLS_OPTIONS_SIZE, 0);                  
                    }
                    else if(menuOption == MENUMAIN_OPTION_GRAPHICS)
                    {
                        sprintf(menuLines[MENUGRAPHICS_OPTIONS_START + 0], "Ambient intensity: %.2f", ambientLightIntensity);
                        sprintf(menuLines[MENUGRAPHICS_OPTIONS_START + 1], "Sun intensity: %.2f", sunLightIntensity);
                        sprintf(menuLines[MENUGRAPHICS_OPTIONS_START + 2], "Sun direction: %.2f", sunLightDirection);
                        
                        
                        setMenu(MENU_GRAPHICS, MENUGRAPHICS_START, MENUGRAPHICS_DESCRIPTION_START, MENUGRAPHICS_DESCRIPTION_SIZE, MENUGRAPHICS_OPTIONS_SIZE, 0);                  
                    }
                    else if(menuOption == MENUMAIN_OPTION_EXIT)
                    {
                        exit = 1;
                    }
                }
            }
            else if(menu == MENU_GRAPHICS)
            {
                int back = 0;
                
                if(isKeyDown(KEY_ESC))
                {
                    back = 1;
                }
                else if(menuOptionSelected)
                {                    
                    if(menuOption == MENUGRAPHICS_OPTION_BACK) { back = 1; }
                }
                else if(menuOptionLeft || menuOptionRight)
                {    
                    if(menuOption == MENUGRAPHICS_OPTION_LIGHT_AMBIENT_INTENSITY)
                    {
                        float step = 1.0f / LIGHT_INTENSITY_STEPS;
                        
                        if(menuOptionRight) { ambientLightIntensity = ambientLightIntensity + step; }
                        else { ambientLightIntensity = ambientLightIntensity - step; }
                        
                        if(ambientLightIntensity < 0) { ambientLightIntensity = 0; }
                        else if(ambientLightIntensity > 1.0f) { ambientLightIntensity = 1.0f; }
                        
                        sprintf(menuLines[MENUGRAPHICS_OPTIONS_START + 0], "Ambient intensity: %.2f", ambientLightIntensity);
                    }
                    else if(menuOption == MENUGRAPHICS_OPTION_LIGHT_DIRECTIONAL_INTENSITY)
                    {
                        float step = 1.0f / LIGHT_INTENSITY_STEPS;
                        
                        if(menuOptionRight) { sunLightIntensity = sunLightIntensity + step; }
                        else { sunLightIntensity = sunLightIntensity - step; }
                        
                        if(sunLightIntensity < 0) { sunLightIntensity = 0; }
                        else if(sunLightIntensity > 1.0f) { sunLightIntensity = 1.0f; }

                        sprintf(menuLines[MENUGRAPHICS_OPTIONS_START + 1], "Sun intensity: %.2f", sunLightIntensity);
                    }
                    else if(menuOption == MENUGRAPHICS_OPTION_LIGHT_DIRECTIONAL_DIRECTION)
                    {
                        float step = 360.0f / SUNLIGHT_DIRECTION_STEPS;
                        
                        if(menuOptionRight) { sunLightDirection = sunLightDirection + step; }
                        else { sunLightDirection = sunLightDirection - step; }
                        
                        if(sunLightDirection < 0) { sunLightDirection += 360.0f; }
                        else if(sunLightDirection > 360.0f) { sunLightDirection -= 360.0f; }

                        sprintf(menuLines[MENUGRAPHICS_OPTIONS_START + 2], "Sun direction: %.2f", sunLightDirection);
                    }
    
                }
                
                if(back)
                {
                    setMenu(MENU_MAIN, MENUMAIN_START, MENUMAIN_DESCRIPTION_START, MENUMAIN_DESCRIPTION_SIZE, MENUMAIN_OPTIONS_SIZE, 2);
                    gameState = GAMESTATE_MAINMENU;
                }
            }
            else if(menu == MENU_CONTROLS)
            {
                int back = 0;
                
                if(isKeyDown(KEY_ESC))
                {
                    back = 1;
                }
                else if(menuOptionSelected)
                {                    
                    if(menuOption == MENUCONTROLS_OPTION_BACK) { back = 1; }
                }
                else if(menuOptionLeft || menuOptionRight)
                {    
                    if(menuOption == MENUCONTROLS_OPTION_MOUSE_SENSITIVITY)
                    {
                        float step = 1.0f / MOUSE_SENSITIVITY_STEPS;
                        
                        if(menuOptionRight) { mouseSensitivity += step; }
                        else { mouseSensitivity -= step; }
                        
                        if(mouseSensitivity < 0) { mouseSensitivity = 0; }
                        else if(mouseSensitivity > 1.0f) { mouseSensitivity = 1.0f; }
                        
                        sprintf(menuLines[MENUCONTROLS_OPTIONS_START + MENUCONTROLS_OPTION_MOUSE_SENSITIVITY], "Mouse sensitivity: %.1f", mouseSensitivity);
                    }
                    else if(menuOption == MENUCONTROLS_OPTION_MODE)
                    {
                        playerControlMode = (playerControlMode == PLAYER_CONTROL_MODE_CLASSIC ? PLAYER_CONTROL_MODE_MODERN : PLAYER_CONTROL_MODE_CLASSIC);
                        
                        sprintf(menuLines[MENUCONTROLS_OPTIONS_START + MENUCONTROLS_OPTION_MODE], "Mode: %s", playerControlMode == PLAYER_CONTROL_MODE_CLASSIC ? "Classic" : "Modern");
                        
                        int isClassic = (playerControlMode == PLAYER_CONTROL_MODE_CLASSIC);
                        const char* desc1 = isClassic ? "Move/Rotate: WASD" : "Move/Strafe: WASD";
                        const char* desc2 = isClassic ? "Strafe: ALT + WASD": "Rotate: Mouse";
                        
                        sprintf(menuLines[MENUMAIN_DESCRIPTION_START + 0], desc1);
                        sprintf(menuLines[MENUMAIN_DESCRIPTION_START + 1], desc2);

                        sprintf(menuLines[MENUCONTROLS_DESCRIPTION_START + 0], desc1);
                        sprintf(menuLines[MENUCONTROLS_DESCRIPTION_START + 1], desc2);                        
                    }
    
                }
                
                if(back)
                {
                    setMenu(MENU_MAIN, MENUMAIN_START, MENUMAIN_DESCRIPTION_START, MENUMAIN_DESCRIPTION_SIZE, MENUMAIN_OPTIONS_SIZE, 1);
                    gameState = GAMESTATE_MAINMENU;
                }
            }            
            
        }
        else // (gameState == GAMESTATE_PLAYING)
        {
            updatePlayer();
            updateView();
            updateHud();
            
            if(isKeyDown(KEY_ESC))
            {
                setMenu(MENU_MAIN, MENUMAIN_START, MENUMAIN_DESCRIPTION_START, MENUMAIN_DESCRIPTION_SIZE, MENUMAIN_OPTIONS_SIZE, 0);             
                gameState = GAMESTATE_MAINMENU;
            }
            
        }
       

        updateScreen();
        //clearScreen();
        
        drawView();

        if(gameState == GAMESTATE_MAINMENU)
        {
            drawMenu();
            drawInfoLine();
        }
        else // gameState == GAMESTATE_PLAYING
        {
            drawHud();
        }
        
        showScreen();
        
        wait((int)(1.0f / SCREEN_FPS * 1000));
                
    }   


}