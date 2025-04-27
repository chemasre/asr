#ifndef _INFOLINE_HPP_
#define _INFOLINE_HPP_

#include <common_header.hpp>

#define INFOLINE_MAX_TEXT_LENGTH 2000

#define INFOLINE_PERIOD 8

#define INFOLINE_COLOR MAKE_COLOR(188,188,188)

extern char infoLineText[INFOLINE_MAX_TEXT_LENGTH];

void initInfoLine();
void updateInfoLine();
void resetInfoLine();
void appendInfoLine(const char* text);
void setInfoLineScrollPosition(int position);
void drawInfoLine();



#endif
