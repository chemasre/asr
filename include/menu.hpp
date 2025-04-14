#ifndef _MENU_HPP_
#define _MENU_HPP_

#include <common_header.hpp>

#define MAX_LINES 100
#define MAX_LINE_SIZE 100

#define MENUTYPE_OPTIONS 0
#define MENUTYPE_MESSAGE 1

#define MENU_COLOR MAKE_COLOR(249,227,79)

extern char menuLines[MAX_LINES][MAX_LINE_SIZE];

extern int menu;
extern int menuType;
extern int menuOption;
extern int menuOptionSelected;
extern int menuOptionLeft;
extern int menuOptionRight;

void initMenu();
void updateMenu();
void setMenu(int _menu, int _titleLine, int _firstTextLine, int _numTextLines, int _numOptions, int _selectedOption);
void drawMenu();



#endif
