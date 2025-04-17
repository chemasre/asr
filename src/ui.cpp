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

int windowStylesEnabled;
int windowColor;
int windowTitleColor;

void initUI()
{
    windowColor = MAKE_COLOR(255, 255, 255);
    windowTitleColor = MAKE_COLOR(255, 255, 255);
    windowStylesEnabled = 0;
}

void enableWindowStyles()
{
    windowStylesEnabled = 1;
}

void disableWindowStyles()
{
    windowStylesEnabled = 0;
}

void drawWindow(int windowX, int windowY, int windowWidth, int windowHeight, char title[], int color)
{
    int color1 = windowStylesEnabled ? windowColor : color;
    int color2 = windowStylesEnabled ? windowTitleColor : color;
    
    // Corners
    
    setScreenCell(windowX, windowY, color1, windowCornerTopLeft);
    setScreenCell(windowX + windowWidth - 1, windowY, color1, windowCornerTopRight);
    setScreenCell(windowX, windowY + windowHeight - 1, color1, windowCornerBottomLeft);
    setScreenCell(windowX + windowWidth - 1, windowY + windowHeight - 1, color1, windowCornerBottomRight);
    
    // Horizontal borders

    fillScreenArea(windowX + 1, windowY, windowWidth - 2, 1, color1, windowBorderHorizontal);
    fillScreenArea(windowX + 1, windowY + windowHeight - 1, windowWidth - 2, 1, color1, windowBorderHorizontal);

    int blankAreaY = windowY + 1;
    int blankAreaHeight = windowHeight - 2;
    
    if(title != NULL)
    {
        // Title borders
        
        setScreenCell(windowX, windowY + 1, color1, windowBorderVertical);
        setScreenCell(windowX + windowWidth - 1, windowY + 1, color1, windowBorderVertical);
        
        // Title
        
        fillScreenArea(windowX + 1, windowY + 1, windowWidth - 2, 1, color1, ' ');
        drawString(color2, title, windowX + 2, windowY + 1);
        
        // Title divider tips
        
        setScreenCell(windowX, windowY + 2, color1, windowDividerTipLeft);
        setScreenCell(windowX + windowWidth - 1, windowY + 2, color1, windowDividerTipRight);
        
        // Title divider interior

        fillScreenArea(windowX + 1, windowY + 2, windowWidth - 2, 1, color1, windowBorderHorizontal);
        
        blankAreaY = blankAreaY + 2;
        blankAreaHeight = blankAreaHeight - 2;
    }
    
    // Vertical borders
    
    fillScreenArea(windowX, blankAreaY, 1, blankAreaHeight, color1, windowBorderVertical);
    fillScreenArea(windowX + windowWidth - 1, blankAreaY, 1, blankAreaHeight, color1, windowBorderVertical);
    
    // Blank area

    fillScreenArea(windowX + 1, blankAreaY, windowWidth - 2, blankAreaHeight, color1, ' ');
    
}

