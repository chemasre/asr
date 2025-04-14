#ifndef _SCREEN_HPP_
#define _SCREEN_HPP_

#include <common_system_header.hpp>
#include <common_header.hpp>

#define SCREEN_RESOLUTION_STEPS 5

#define SCREEN_FPS 24
#define SCREEN_MIN_WINDOW_WIDTH 960
#define SCREEN_MIN_WINDOW_HEIGHT 480

#define MAKE_COLOR(r,g,b) (((0xFF & r) << 16) | ((0xFF & g) << 8) | (0xFF & b))
#define GET_COLOR_R(c) (0xFF & ((c) >> 16))
#define GET_COLOR_G(c) (0xFF & ((c) >> 8))
#define GET_COLOR_B(c) (0xFF & (c))


struct ScreenCell
{
    char string[sizeof(char) * 32];
    int color;
    char character;
};

extern int frameCounter;
extern int screenWidth;
extern int screenHeight;
extern int fontWidth;
extern int fontHeight;
extern ScreenCell** screen;
extern ScreenCell* screenLine;

void setScreenTitle(char t[]);
void drawString(int color, char s[], int x, int y);
void setScreenCursorPosition(int x, int y);
void setScreenCell(int cellX, int cellY, int color, char c);
ScreenCell getScreenCell(int cellX, int cellY, int defaultColor, char defaultChar);
void fillScreenArea(int areaX, int blankAreaY, int areaWidth, int blankAreaHeight, int color, char c);
void setClearColor(int color);
void clearScreen();
void showScreen();
void initScreen();
void updateScreen();
void resizeScreen(int windowWidth, int windowHeight);

#endif
