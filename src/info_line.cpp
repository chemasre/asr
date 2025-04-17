#include "info_line.hpp"
#include <screen.hpp>

char infoLineText[INFOLINE_MAX_TEXT_LENGTH];

float scrollPosition;
int infoLineLength;

void initInfoLine()
{
    infoLineText[0] = '\0';
    scrollPosition = 0;
    infoLineLength = 0;
}

void updateInfoLine()
{
    scrollPosition += (screenWidth / INFOLINE_PERIOD) * (1.0f / SCREEN_FPS);
    
    if(scrollPosition >= screenWidth + infoLineLength)
    {
        scrollPosition = 0;
    }
}

void setInfoLineScrollPosition(int position)
{
    scrollPosition = position;
}

void resetInfoLine()
{
    sprintf(infoLineText, "");
    scrollPosition = 0;
    
}

void appendInfoLine(const char* text)
{
    strcat(infoLineText, text);
    infoLineLength += strlen(text);
}

void drawInfoLine()
{
    fillScreenArea(0, screenHeight - 1, screenWidth, 1, MAKE_COLOR(0, 0, 0), ' ');
    drawString(INFOLINE_COLOR, infoLineText, screenWidth - scrollPosition, screenHeight - 1);
}

