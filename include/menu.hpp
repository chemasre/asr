#ifndef _MENU_HPP_
#define _MENU_HPP_

#include <common_header.hpp>

#define MAX_LINES 100
#define MAX_LINE_SIZE 100

extern char menuLines[MAX_LINES][MAX_LINE_SIZE];

extern int menu;
extern int menuOption;
extern int menuOptionSelected;
extern int menuOptionLeft;
extern int menuOptionRight;

void initMenu();
void updateMenu();
void setMenu(int _menu, int _titleLine, int _firstOptionLine, int _numOptions, int _selectedOption);
void drawMenu();



#endif
