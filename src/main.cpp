#include <common_header.hpp>
#include <screen.hpp>
#include <input.hpp>
#include <player.hpp>
#include <map.hpp>
#include <ui.hpp>

/**************************************************************
                   
                   A SIMPLE RAYCASTER
   
**************************************************************/

void main()
{
    int gameStarted;
    
    gameStarted = 0;
        
    frameCounter = 0;

    initScreen();
    
    while(!isKeyPressed(27))
    {
        if(gameStarted)
        {
            updatePlayer();
        }
        else
        {
            if(isKeyPressed(' '))
            {
                initPlayer();
                gameStarted = 1;
            }
        }
        
        updateScreen();
        
        clearScreen();
        
        if(gameStarted)
        {
            drawView();
            drawHud();
        }
        else
        {
            int x = screenWidth/ 2 - 12;
            int y = screenHeight/2 - 5;
            
            drawWindow(x, y, 24, 8, "A simple Raycaster");
            drawString("   Move/Turn: WASD    ", x + 1, y + 3);
            drawString("   Quit: ESC          ", x + 1, y + 4);
            drawString("                      ", x + 1, y + 5);
            if(frameCounter % 8 < 4) { drawString(" Press space to start ", x + 1, y + 6); }
        }
        
        showScreen();
        
        Sleep((int)(1.0f / SCREEN_FPS * 1000));
        
        frameCounter ++;
        
    }   


}