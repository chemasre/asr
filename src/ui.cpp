#include <ui.hpp>
#include <player.hpp>
#include <map.hpp>
#include <view.hpp>

const int windowCornerTopLeft = '.';
const int windowCornerTopRight = '.';
const int windowCornerBottomLeft = '`';
const int windowCornerBottomRight = '`';
const int windowBorderHorizontal = '-';
const int windowBorderVertical = '|';
const int windowDividerTipLeft = '|';
const int windowDividerTipRight = '|';

void drawWindow(int windowX, int windowY, int windowWidth, int windowHeight, char title[], int color)
{
    // Corners
    
    setScreenCell(windowX, windowY, color, windowCornerTopLeft);
    setScreenCell(windowX + windowWidth - 1, windowY, color, windowCornerTopRight);
    setScreenCell(windowX, windowY + windowHeight - 1, color, windowCornerBottomLeft);
    setScreenCell(windowX + windowWidth - 1, windowY + windowHeight - 1, color, windowCornerBottomRight);
    
    // Horizontal borders

    fillScreenArea(windowX + 1, windowY, windowWidth - 2, 1, color, windowBorderHorizontal);
    fillScreenArea(windowX + 1, windowY + windowHeight - 1, windowWidth - 2, 1, color, windowBorderHorizontal);

    int blankAreaY = windowY + 1;
    int blankAreaHeight = windowHeight - 2;
    
    if(title != NULL)
    {
        // Title borders
        
        setScreenCell(windowX, windowY + 1, color, windowBorderVertical);
        setScreenCell(windowX + windowWidth - 1, windowY + 1, color, windowBorderVertical);
        
        // Title
        
        fillScreenArea(windowX + 1, windowY + 1, windowWidth - 2, 1, color, ' ');
        drawString(color, title, windowX + 2, windowY + 1);
        
        // Title divider tips
        
        setScreenCell(windowX, windowY + 2, color, windowDividerTipLeft);
        setScreenCell(windowX + windowWidth - 1, windowY + 2, color, windowDividerTipRight);
        
        // Title divider interior

        fillScreenArea(windowX + 1, windowY + 2, windowWidth - 2, 1, color, windowBorderHorizontal);
        
        blankAreaY = blankAreaY + 2;
        blankAreaHeight = blankAreaHeight - 2;
    }
    
    // Vertical borders
    
    fillScreenArea(windowX, blankAreaY, 1, blankAreaHeight, color, windowBorderVertical);
    fillScreenArea(windowX + windowWidth - 1, blankAreaY, 1, blankAreaHeight, color, windowBorderVertical);
    
    // Blank area

    fillScreenArea(windowX + 1, blankAreaY, windowWidth - 2, blankAreaHeight, color, ' ');
    
}

