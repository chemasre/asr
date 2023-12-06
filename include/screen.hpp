#ifndef _SCREEN_HPP_
#define _SCREEN_HPP_

#include <common_system_header.hpp>

#define SCREEN_HEIGHT 29
#define SCREEN_HEIGHT2WIDTH 4
#define MAX_SCREEN_HEIGHT 60
#define MIN_SCREEN_HEIGHT 10
#define MAX_SCREEN_WIDTH (MAX_SCREEN_HEIGHT * SCREEN_HEIGHT2WIDTH)
#define SCREEN_RESOLUTION_STEPS 5

#define SCREEN_FPS 10

extern int frameCounter;
extern int screenWidth;
extern int screenHeight;
extern char screen[MAX_SCREEN_HEIGHT][MAX_SCREEN_WIDTH + 1];
extern char screenLine[MAX_SCREEN_WIDTH + 1];


void drawString(char s[], int x, int y);
void setScreenCursorPosition(int x, int y);
void setScreenCell(int cellX, int cellY, char c);
void fillScreenArea(int areaX, int blankAreaY, int areaWidth, int blankAreaHeight, char c);
void clearScreen();
void showScreen();
void initScreen();
void updateScreen();

#endif
