#include <common_header.hpp>
#include <input.hpp>
#include <screen.hpp>
#include <ui.hpp>
#include <system.hpp>
#include <game.hpp>
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
#define MENU_CONFIG 1

#define MENU_MAIN_OPTION_CONFIG 1

#define MENUMAIN_TITLE 0
#define MENUMAIN_FIRST_TEXTLINE (MENUMAIN_TITLE + 1)
#define MENUMAIN_TEXTLINES 2
#define MENUMAIN_OPTIONS 3

#define MENUCONFIG_TITLE (MENUMAIN_FIRST_TEXTLINE + MENUMAIN_TEXTLINES + MENUMAIN_OPTIONS)
#define MENUCONFIG_FIRST_TEXTLINE (MENUCONFIG_TITLE + 1)
#define MENUCONFIG_TEXTLINES 0
#define MENUCONFIG_OPTIONS 4

#define SCREEN_TITLE "A Simple Raycaster"

#define BACKGROUND_COLOR MAKE_COLOR(0, 0, 0)


void main()
{
        
    initInput();
    initScreen();
    initSprites();
    initMap();
    initTextures();
    initView();
    initPlayer();
    initMenu();
    
    setScreenTitle(SCREEN_TITLE);
    setClearColor(BACKGROUND_COLOR);
   
    sprintf(menuLines[MENUCONFIG_FIRST_TEXTLINE + MENUCONFIG_TEXTLINES + 0], "Ambient intensity: %.2f", ambientLightIntensity);
    sprintf(menuLines[MENUCONFIG_FIRST_TEXTLINE + MENUCONFIG_TEXTLINES + 1], "Sun intensity: %.2f", sunLightIntensity);
    sprintf(menuLines[MENUCONFIG_FIRST_TEXTLINE + MENUCONFIG_TEXTLINES + 2], "Sun direction: %.2f", sunLightDirection);
    
    int gameState = GAMESTATE_MAINMENU;
    int exit = 0;
    
    setMenu(MENU_MAIN, MENUMAIN_TITLE, MENUMAIN_FIRST_TEXTLINE, MENUMAIN_TEXTLINES, MENUMAIN_OPTIONS, 0);

    while(!exit)
    {
        if(gameState == GAMESTATE_MAINMENU)
        {
            updateMenu();
            
            if(menu == MENU_MAIN)
            {
                if(isKeyDown(KEY_ESC))
                {
                    exit = 1;
                }
                else if(menuOptionSelected)
                {
                    if(menuOption == 0)
                    {
                        gameState = GAMESTATE_PLAYING;
                    }
                    else if(menuOption == 1)
                    {
                        setMenu(MENU_CONFIG, MENUCONFIG_TITLE, MENUCONFIG_FIRST_TEXTLINE, MENUCONFIG_TEXTLINES, MENUCONFIG_OPTIONS, 0);                    
                    }
                    else if(menuOption == 2)
                    {
                        exit = 1;
                    }
                }
            }
            else if(menu == MENU_CONFIG)
            {
                int back = 0;
                
                if(isKeyDown(KEY_ESC))
                {
                    back = 1;
                }
                else if(menuOptionSelected)
                {                    
                    if(menuOption == 3) { back = 1; }
                }
                else if(menuOptionLeft || menuOptionRight)
                {    
                    if(menuOption == 0)
                    {
                        float step = 1.0f / LIGHT_INTENSITY_STEPS;
                        
                        if(menuOptionRight) { ambientLightIntensity = ambientLightIntensity + step; }
                        else { ambientLightIntensity = ambientLightIntensity - step; }
                        
                        if(ambientLightIntensity < 0) { ambientLightIntensity = 0; }
                        else if(ambientLightIntensity > 1.0f) { ambientLightIntensity = 1.0f; }
                        
                        sprintf(menuLines[MENUCONFIG_FIRST_TEXTLINE + MENUCONFIG_TEXTLINES + 0], "Ambient intensity: %.2f", ambientLightIntensity);
                    }
                    else if(menuOption == 1)
                    {
                        float step = 1.0f / LIGHT_INTENSITY_STEPS;
                        
                        if(menuOptionRight) { sunLightIntensity = sunLightIntensity + step; }
                        else { sunLightIntensity = sunLightIntensity - step; }
                        
                        if(sunLightIntensity < 0) { sunLightIntensity = 0; }
                        else if(sunLightIntensity > 1.0f) { sunLightIntensity = 1.0f; }

                        sprintf(menuLines[MENUCONFIG_FIRST_TEXTLINE + MENUCONFIG_TEXTLINES + 1], "Sun intensity: %.2f", sunLightIntensity);
                    }
                    else if(menuOption == 2)
                    {
                        float step = 360.0f / SUNLIGHT_DIRECTION_STEPS;
                        
                        if(menuOptionRight) { sunLightDirection = sunLightDirection + step; }
                        else { sunLightDirection = sunLightDirection - step; }
                        
                        if(sunLightDirection < 0) { sunLightDirection += 360.0f; }
                        else if(sunLightDirection > 360.0f) { sunLightDirection -= 360.0f; }

                        sprintf(menuLines[MENUCONFIG_FIRST_TEXTLINE + MENUCONFIG_TEXTLINES + 2], "Sun direction: %.2f", sunLightDirection);
                    }
    
                }
                
                if(back)
                {
                    setMenu(MENU_MAIN, MENUMAIN_TITLE, MENUMAIN_FIRST_TEXTLINE, MENUMAIN_TEXTLINES, MENUMAIN_OPTIONS, 1);
                    gameState = GAMESTATE_MAINMENU;
                }
            }
            
        }
        else // (gameState == GAMESTATE_PLAYING)
        {
            updatePlayer();
            updateView();
            
            if(isKeyDown(KEY_ESC))
            {
                setMenu(MENU_MAIN, MENUMAIN_TITLE, MENUMAIN_FIRST_TEXTLINE, MENUMAIN_TEXTLINES, MENUMAIN_OPTIONS, 0);                
                gameState = GAMESTATE_MAINMENU;
            }
            
        }
       

        updateScreen();
        //clearScreen();
        
        drawView();

        if(gameState == GAMESTATE_MAINMENU)
        {
            drawMenu();
        }
        else // gameState == GAMESTATE_PLAYING
        {
            drawHud();
        }
        
        showScreen();
        
        wait((int)(1.0f / SCREEN_FPS * 1000));
                
    }   


}