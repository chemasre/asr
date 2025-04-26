#ifndef _UI_HPP_
#define _UI_HPP_

#include <common_header.hpp>

extern int windowColor;
extern int textAreaColor;
extern int windowTitleColor;
extern int windowCornerTopLeft;
extern int windowCornerTopRight;
extern int windowCornerBottomLeft;
extern int windowCornerBottomRight;
extern int windowBorderHorizontal;
extern int windowBorderVertical;
extern int windowDividerTipLeft;
extern int windowDividerTipRight;

void initUI();
void enableWindowStyles();
void disableWindowStyles();
void drawWindow(int windowX, int windowY, int windowWidth, int windowHeight, char title[], int color);
void drawTextArea(int x, int y, int w, int h, const char text[], int color);

#endif
