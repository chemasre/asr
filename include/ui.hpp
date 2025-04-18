#ifndef _UI_HPP_
#define _UI_HPP_

#include <common_header.hpp>

extern int windowColor;
extern int windowTitleColor;

void initUI();
void enableWindowStyles();
void disableWindowStyles();
void drawWindow(int windowX, int windowY, int windowWidth, int windowHeight, char title[], int color);

#endif
