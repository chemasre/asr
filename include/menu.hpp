#ifndef _MENU_HPP_
#define _MENU_HPP_

#include <common_header.hpp>

#define MAX_LINES 100
#define MAX_LINE_SIZE 100

#define MENU_TITLE_COLOR MAKE_COLOR(237,222,198)
#define MENU_WINDOW_COLOR MAKE_COLOR(241,159,77)
#define MENU_OPTION_COLOR MAKE_COLOR(241,159,77)
#define MENU_OPTION_SELECTED_COLOR MAKE_COLOR(255,255,255)

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
