#ifndef _SCREEN_HPP_
#define _SCREEN_HPP_

#include <common_system_header.hpp>
#include <common_header.hpp>

#define SCREEN_RESOLUTION_STEPS 5

#define SCREEN_FPS 24
#define SCREEN_MIN_WINDOW_WIDTH 960
#define SCREEN_MIN_WINDOW_HEIGHT 480

extern int frameCounter;
extern int screenWidth;
extern int screenHeight;
extern int fontWidth;
extern int fontHeight;
extern char** screen;
extern char *screenLine;

void setScreenTitle(char t[]);
void drawString(char s[], int x, int y);
void setScreenCursorPosition(int x, int y);
void setScreenCell(int cellX, int cellY, char c);
void fillScreenArea(int areaX, int blankAreaY, int areaWidth, int blankAreaHeight, char c);
void clearScreen();
void showScreen();
void initScreen();
void updateScreen();

#endif
