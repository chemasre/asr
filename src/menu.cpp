#include <menu.hpp>
#include <ui.hpp>

char line[100];

int menu;
int menuOption;

int menuOptionSelected;
int menuOptionLeft;
int menuOptionRight;

int titleLine;
int firstOptionLine;
int numOptions;

char menuLines[MAX_LINES][MAX_LINE_SIZE] = {
    "A simple Raycaster",
    "Play",
    "Settings",
    "Exit",
    "Settings",
    "Ambient intensity: %.2f",
    "Sun intensity: %.2f",
    "Sun direction: %.2f",
    "Back",    
};

void initMenu()
{
    menuOption = 0;

    menuOptionSelected = 0;
    menuOptionLeft = 0;
    menuOptionRight = 0;

    titleLine = 0;
    firstOptionLine = 0;
    numOptions = 0;
    
}


void updateMenu()
{
    int menuOptionUp = isKeyDown('W');
    int menuOptionDown = isKeyDown('S');
    
    menuOptionLeft = isKeyDown('A');
    menuOptionRight = isKeyDown('D');
    menuOptionSelected = isKeyDown(KEY_SPACE);
    
    if(menuOptionUp) { menuOption = menuOption - 1; if(menuOption < 0) { menuOption = numOptions - 1; } }
    else if(menuOptionDown) { menuOption = menuOption + 1; if(menuOption >= numOptions) { menuOption = 0; } }
}

void setMenu(int _menu, int _titleLine, int _firstOptionLine, int _numOptions, int _selectedOption)
{
    menu = _menu;
    titleLine = _titleLine;
    firstOptionLine = _firstOptionLine;
    numOptions = _numOptions;
    menuOption = _selectedOption;
}

void drawMenu()
{
    int maxLineLength = strlen(menuLines[titleLine]);
    
    for(int i = 0; i < numOptions; i ++)
    {
        if(strlen(menuLines[firstOptionLine + i]) > maxLineLength) { maxLineLength = strlen(menuLines[firstOptionLine + i]); }
    }
    
    int width = maxLineLength + 6;
    int height = 4 + numOptions + 2;
            
    int x = screenWidth / 2 - width / 2;
    int y = screenHeight / 2 - height / 2;
    
    drawWindow(x, y, width, height, menuLines[titleLine]);
    
    for(int i = 0; i < numOptions; i ++)
    {
        sprintf(line, i == menuOption ? "<%s>" : "%s", menuLines[firstOptionLine + i]);
        drawString(line, x + 1 - strlen(line) / 2 + (width - 4) / 2, y + 4 + i);
    }
}


