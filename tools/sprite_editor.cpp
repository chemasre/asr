#include <stdio.h>
#include "sprite_editor.hpp"

#include <input.hpp>
#include <menu.hpp>
#include <ui.hpp>
#include <screen.hpp>
#include <system.hpp>

#define VERSION_STRING_SIZE 20
#define VERSION_STRING_PATTERN "  v%d.%d.%d"

#define CELLS_WIDTH 70
#define CELLS_HEIGHT 40

#define COLOR_SELECTED MAKE_COLOR(255,255,255)
#define COLOR_UNSELECTED MAKE_COLOR(127,127,127)
#define COLOR_ERROR MAKE_COLOR(255,0,0)

#define SPRITE_FILE_PATTERN "sprite%02d.spr"

#define ASCII_FIRST 32
#define ASCII_COUNT 95

#define ASCII_PALETTE_WIDTH 12
#define ASCII_PALETTE_HEIGHT (ASCII_COUNT / ASCII_PALETTE_WIDTH + 1)

#define ASCII_PALETTE_POSITION_X (CELLS_WIDTH - ASCII_PALETTE_WIDTH - 2)
#define ASCII_PALETTE_POSITION_Y 4

#define COLOR_PALETTE1_WIDTH 12
#define COLOR_PALETTE1_HEIGHT 12

#define COLOR_PALETTE1_POSITION_X (CELLS_WIDTH - COLOR_PALETTE1_WIDTH - 2)
#define COLOR_PALETTE1_POSITION_Y (ASCII_PALETTE_POSITION_Y + ASCII_PALETTE_HEIGHT + 2 + 2)

#define COLOR_PALETTE2_POSITION_X (CELLS_WIDTH - COLOR_PALETTE2_WIDTH - 2)
#define COLOR_PALETTE2_POSITION_Y (COLOR_PALETTE1_POSITION_Y + COLOR_PALETTE1_HEIGHT + 2 + 2)

#define COLOR_PALETTE2_WIDTH 12
#define COLOR_PALETTE2_HEIGHT 1

#define INFO_POSITION_X (CELLS_WIDTH - INFO_WIDTH - 2)
#define INFO_POSITION_Y (COLOR_PALETTE2_POSITION_Y  + COLOR_PALETTE2_HEIGHT + 2 + 2)

#define INFO_WIDTH 12
#define INFO_HEIGHT 1

#define TOOL_DRAW 0
#define TOOL_PICK 1

#define TOOLS_COUNT 2
#define TOOLS_WIDTH 10
#define TOOLS_HEIGHT TOOLS_COUNT
#define TOOLS_POSITION_X 3
#define TOOLS_POSITION_Y 4

#define MODE_ALL   0
#define MODE_CHAR  1
#define MODE_COLOR 2

#define MODES_COUNT 3
#define MODES_WIDTH 10
#define MODES_HEIGHT MODES_COUNT
#define MODES_POSITION_X 3
#define MODES_POSITION_Y (TOOLS_POSITION_Y + TOOLS_HEIGHT + 5)

#define COMMAND_NEW 0
#define COMMAND_LOAD 1
#define COMMAND_SAVE 2
#define COMMAND_EXIT 3

#define COMMAND_HIGHLIGHT_TIME (0.5f)

#define COMMANDS_COUNT 4

#define COMMANDS_WIDTH 10
#define COMMANDS_HEIGHT COMMANDS_COUNT
#define COMMANDS_POSITION_X 3
#define COMMANDS_POSITION_Y (MODES_POSITION_Y + MODES_HEIGHT + 5)

#define ABOUT_WIDTH 10
#define ABOUT_HEIGHT 5
#define ABOUT_POSITION_X 3
#define ABOUT_POSITION_Y (CELLS_HEIGHT - ABOUT_HEIGHT - 1)

#define SLOTS_COUNT 16
#define SLOTS_WIDTH (SLOTS_COUNT * 2)
#define SLOTS_HEIGHT 1
#define SLOTS_POSITION_X (TOOLS_POSITION_X + TOOLS_WIDTH + 1 + 5)
#define SLOTS_POSITION_Y (CELLS_HEIGHT - SLOTS_HEIGHT - 1)

char versionString[VERSION_STRING_SIZE];

char cursorChar;
int cursorColor;
int cursorCellX;
int cursorCellY;

int selectedTool;
int selectedMode;

int selectedSlot;
int selectedRedLevel;
int selectedGreenLevel;
int selectedBlueLevel;
int selectedAsciiLevel;

int drawAreaPosX = TOOLS_POSITION_X + TOOLS_WIDTH + 1 + 5;
int drawAreaPosY = 4;
int drawAreaWidth = 32;
int drawAreaHeight = 32;

char workPath[MAX_PATH_LENGTH];

ScreenCell** sprite[SLOTS_COUNT];


float commandHighlightedTimer;
int commandHighlighted;
int commandHighlightedError;



void drawUI()
{
    drawWindow(ASCII_PALETTE_POSITION_X - 1, ASCII_PALETTE_POSITION_Y - 3, ASCII_PALETTE_WIDTH + 2, ASCII_PALETTE_HEIGHT + 4, "ASCII", MAKE_COLOR(255, 255, 0));
    
    drawWindow(COLOR_PALETTE1_POSITION_X - 1, COLOR_PALETTE1_POSITION_Y - 3, COLOR_PALETTE1_WIDTH + 2, COLOR_PALETTE1_HEIGHT + 4, "COLOR", MAKE_COLOR(255, 255, 0));    

    drawWindow(COLOR_PALETTE2_POSITION_X - 1, COLOR_PALETTE2_POSITION_Y - 3, COLOR_PALETTE2_WIDTH + 2, COLOR_PALETTE2_HEIGHT + 4, "COLOR", MAKE_COLOR(255, 255, 0));    
    
    drawWindow(INFO_POSITION_X - 1, INFO_POSITION_Y - 3, INFO_WIDTH + 2, INFO_HEIGHT + 4, "INFO", MAKE_COLOR(255, 255, 0));  

    drawWindow(drawAreaPosX - 1, drawAreaPosY - 3, drawAreaWidth + 2, drawAreaHeight + 4, "SPRITE", MAKE_COLOR(255, 255, 0)); 

    drawWindow(SLOTS_POSITION_X - 1, SLOTS_POSITION_Y - 1, SLOTS_WIDTH + 2, SLOTS_HEIGHT + 2, NULL, MAKE_COLOR(255, 255, 0));
    
    drawWindow(TOOLS_POSITION_X - 1, TOOLS_POSITION_Y - 3, TOOLS_WIDTH + 2, TOOLS_HEIGHT + 4, "TOOLS", MAKE_COLOR(255, 255, 0));    

    drawWindow(MODES_POSITION_X - 1, MODES_POSITION_Y - 3, MODES_WIDTH + 2, MODES_HEIGHT + 4, "MODES", MAKE_COLOR(255, 255, 0));    

    drawWindow(COMMANDS_POSITION_X - 1, COMMANDS_POSITION_Y - 3, COMMANDS_WIDTH + 2, COMMANDS_HEIGHT + 4, "COMMAND", MAKE_COLOR(255, 255, 0)); 
    
    drawWindow(ABOUT_POSITION_X - 1, ABOUT_POSITION_Y - 1, ABOUT_WIDTH + 2, ABOUT_HEIGHT + 2, NULL, COLOR_UNSELECTED);     
    
    char asciiLevel = 0;
    
    while(asciiLevel < ASCII_COUNT)
    {
        int color;
        if(asciiLevel == selectedAsciiLevel)
        { color = COLOR_SELECTED; }
        else { color = COLOR_UNSELECTED; }
        
        setScreenCell(ASCII_PALETTE_POSITION_X + asciiLevel % ASCII_PALETTE_WIDTH,
                     ASCII_PALETTE_POSITION_Y + asciiLevel / ASCII_PALETTE_WIDTH, color, asciiLevel + ASCII_FIRST);
        
        asciiLevel++;
    }
    
    
    for(int y = 0; y < COLOR_PALETTE1_HEIGHT; y++)
    {
        for(int x = 0; x < COLOR_PALETTE1_WIDTH; x++)
        {
            int r = (int)(x * (255.0f / COLOR_PALETTE1_WIDTH));
            int g = (int)(y * (255.0f / COLOR_PALETTE1_HEIGHT));
            int b = (int)(selectedBlueLevel * (255.0f / COLOR_PALETTE2_WIDTH));
            
            int color;
            if(selectedRedLevel == x && selectedGreenLevel == y) { color = COLOR_SELECTED; }
            else { color = MAKE_COLOR(r, g, b); }            
            
            setScreenCell(COLOR_PALETTE1_POSITION_X + x, COLOR_PALETTE1_POSITION_Y + y, color, '#');
        }
    }
    
    for(int x = 0; x < COLOR_PALETTE2_WIDTH; x++)
    {
        int r = 0;
        int g = 0;
        int b = (int)(x * (255.0f / COLOR_PALETTE2_WIDTH));
        
        int color;
        if(selectedBlueLevel == x) { color = COLOR_SELECTED; }
        else { color = MAKE_COLOR(r, g, b); } 
        
        setScreenCell(COLOR_PALETTE2_POSITION_X + x, COLOR_PALETTE2_POSITION_Y, color, '#');
    } 

    for(int x = 0; x < INFO_WIDTH; x++)
    {
        int r = (int)(selectedRedLevel * (255.0f / COLOR_PALETTE1_WIDTH));
        int g = (int)(selectedGreenLevel * (255.0f / COLOR_PALETTE1_HEIGHT));
        int b = (int)(selectedBlueLevel * (255.0f / COLOR_PALETTE2_WIDTH));
        
        char c = ASCII_FIRST + selectedAsciiLevel;
        
        setScreenCell(INFO_POSITION_X + x, INFO_POSITION_Y, MAKE_COLOR(r, g, b), c);
    } 

    for(int y = 0; y < drawAreaHeight; y++)
    {
        for(int x = 0; x < drawAreaWidth; x ++)
        {
            setScreenCell(drawAreaPosX + x, drawAreaPosY + y, sprite[selectedSlot][y][x].color, sprite[selectedSlot][y][x].character);
        }
    }

    for(int x = 0; x < SLOTS_COUNT; x ++)
    {
        int color;
        if(x == selectedSlot) { color = COLOR_SELECTED; }
        else { color = COLOR_UNSELECTED; }             

        setScreenCell(SLOTS_POSITION_X + x * 2, SLOTS_POSITION_Y, color, x <= 9 ? '0' + x : 'A' + x - 10);
    }
    
    drawString(selectedTool == TOOL_DRAW ? COLOR_SELECTED : COLOR_UNSELECTED, " DRAW", TOOLS_POSITION_X, TOOLS_POSITION_Y + 0);
    drawString(selectedTool == TOOL_PICK ? COLOR_SELECTED : COLOR_UNSELECTED, " PICK", TOOLS_POSITION_X, TOOLS_POSITION_Y + 1);

    drawString(selectedMode == MODE_ALL ? COLOR_SELECTED : COLOR_UNSELECTED, " ALL", MODES_POSITION_X, MODES_POSITION_Y + 0);
    drawString(selectedMode == MODE_CHAR ? COLOR_SELECTED : COLOR_UNSELECTED, " ASCII", MODES_POSITION_X, MODES_POSITION_Y + 1);
    drawString(selectedMode == MODE_COLOR ? COLOR_SELECTED : COLOR_UNSELECTED, " COLOR", MODES_POSITION_X, MODES_POSITION_Y + 2);

    int highlight = (commandHighlightedTimer > 0);
    
    int highlightNew = highlight && (commandHighlighted == COMMAND_NEW);
    int highlightLoad = highlight && (commandHighlighted == COMMAND_LOAD);
    int highlightSave = highlight && (commandHighlighted == COMMAND_SAVE);
    int highlightColor = commandHighlightedError ? COLOR_ERROR : COLOR_SELECTED; 
    
    drawString(COLOR_SELECTED, " F1", COMMANDS_POSITION_X, COMMANDS_POSITION_Y + 0);
    drawString(highlightNew ? highlightColor : COLOR_UNSELECTED, "NEW", COMMANDS_POSITION_X + 5, COMMANDS_POSITION_Y + 0);

    drawString(COLOR_SELECTED, " F2", COMMANDS_POSITION_X, COMMANDS_POSITION_Y + 1);
    drawString(highlightLoad ? highlightColor : COLOR_UNSELECTED, "LOAD", COMMANDS_POSITION_X + 5, COMMANDS_POSITION_Y + 1);

    drawString(COLOR_SELECTED, " F3", COMMANDS_POSITION_X, COMMANDS_POSITION_Y + 2);
    drawString(highlightSave ? highlightColor : COLOR_UNSELECTED, "SAVE", COMMANDS_POSITION_X + 5, COMMANDS_POSITION_Y + 2);

    drawString(COLOR_SELECTED, " ESC", COMMANDS_POSITION_X, COMMANDS_POSITION_Y + 3);
    drawString(COLOR_UNSELECTED, "EXIT", COMMANDS_POSITION_X + 5, COMMANDS_POSITION_Y + 3);
    
    sprintf(versionString, VERSION_STRING_PATTERN, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    
    drawString(COLOR_UNSELECTED, "  SPRITE  ", ABOUT_POSITION_X, ABOUT_POSITION_Y + 0);
    drawString(COLOR_UNSELECTED, "  EDITOR  ", ABOUT_POSITION_X, ABOUT_POSITION_Y + 1);
    drawString(COLOR_UNSELECTED, versionString, ABOUT_POSITION_X, ABOUT_POSITION_Y + 2);
    drawString(COLOR_UNSELECTED, "    BY    ", ABOUT_POSITION_X, ABOUT_POSITION_Y + 3);
    drawString(COLOR_UNSELECTED, " JM SOLIS ", ABOUT_POSITION_X, ABOUT_POSITION_Y + 4);

}

int isInsideRect(int pX, int pY, int rectX, int rectY, int rectW, int rectH)
{
    if(pX >= rectX && pX <= rectX + rectW - 1 && pY >= rectY && pY <= rectY + rectH - 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int tryLoadSprite(int slot)
{
    int success;
    
    char fileName[MAX_PATH_LENGTH];
    sprintf(fileName, SPRITE_FILE_PATTERN, slot);
    char filePath[MAX_PATH_LENGTH];
    sprintf(filePath, "%s%s", workPath, fileName);
    
    FILE* file = fopen(filePath, "rb");
    
    if(file != 0)
    {
        fread(&drawAreaWidth, sizeof(int), 1, file);
        fread(&drawAreaHeight, sizeof(int), 1, file);
        
        for(int y = 0; y < drawAreaHeight; y++)
        {
            for(int x = 0; x < drawAreaWidth; x++)
            {
                fread(&sprite[slot][y][x].character, sizeof(char), 1, file);
                fread(&sprite[slot][y][x].color, sizeof(int), 1, file);
            }
        }

        fclose(file);
        
        success = 1;

    }    
    else
    {
        success = 0;
    }
    
    return success;
}

int trySaveSprite(int slot)
{
    int success;
    
    char fileName[MAX_PATH_LENGTH];
    sprintf(fileName, SPRITE_FILE_PATTERN, slot);
    char filePath[MAX_PATH_LENGTH];
    sprintf(filePath, "%s%s", workPath, fileName);
    
    FILE* file = fopen(filePath, "wb");
    
    if(file != 0)
    {
        fwrite(&drawAreaWidth, sizeof(int), 1, file);
        fwrite(&drawAreaHeight, sizeof(int), 1, file);
        
        for(int y = 0; y < drawAreaHeight; y++)
        {
            for(int x = 0; x < drawAreaWidth; x++)
            {
                fwrite(&sprite[slot][y][x].character, sizeof(char), 1, file);
                fwrite(&sprite[slot][y][x].color, sizeof(int), 1, file);
            }
        }

        fclose(file);
        
        success = 1;
    } 
    else
    {
        success = 0;
    }    
    
    return success;
}

int main(int argc, char* argv[])
{
    if(argc >= 2)
    {
        strcpy(workPath, argv[1]);
    }
    else
    {
        strcpy(workPath, "");
    }
    
    initInput();
    initScreen();
    initMenu();
    
    setScreenTitle("Sprite editor");
    
    resizeScreen(CELLS_WIDTH * fontWidth, CELLS_HEIGHT * fontHeight);
    
    int exit = 0;
    
    cursorChar = 'O';
    cursorColor = MAKE_COLOR(255, 255, 255);
    
    selectedTool = TOOL_DRAW;
    selectedMode = MODE_ALL;
    
    selectedSlot = 0;
    selectedRedLevel = COLOR_PALETTE1_WIDTH - 1;
    selectedGreenLevel = COLOR_PALETTE1_HEIGHT - 1;
    selectedBlueLevel = COLOR_PALETTE2_WIDTH - 1;
    selectedAsciiLevel = '#' - ASCII_FIRST;
    
    commandHighlighted = 0;
    commandHighlightedTimer = 0;
    commandHighlightedError = 0;
    
    for(int s = 0; s < SLOTS_COUNT; s++)
    {
        sprite[s] = (ScreenCell**)malloc(sizeof(ScreenCell*) * drawAreaHeight);
        for(int i = 0; i < drawAreaHeight; i ++)
        { 
            sprite[s][i] = (ScreenCell*)malloc(sizeof(ScreenCell) * drawAreaWidth);
            
            for(int j = 0; j < drawAreaWidth; j++)
            {
                sprite[s][i][j].color = 0;
                sprite[s][i][j].character = ' ';
            }
        }
    }
    
    for(int i = 0; i < SLOTS_COUNT; i++)
    {
        tryLoadSprite(i);
    }
        
    while(!exit)
    {
        
        if(isKeyDown(KEY_ESC))
        {
            exit = 1;
        }
        else if(isKeyDown(KEY_F1))
        {
            for(int y = 0; y < drawAreaHeight; y++)
            {
                for(int x = 0; x < drawAreaWidth; x++)
                {
                    sprite[selectedSlot][y][x].character = ' ';
                    sprite[selectedSlot][y][x].color = 0;                    
                }
            }
            
            commandHighlighted = COMMAND_NEW;
            commandHighlightedError = 0;
            commandHighlightedTimer = COMMAND_HIGHLIGHT_TIME;
            
            
        }
        else if(isKeyDown(KEY_F2))
        {
            int success;
            
            success = tryLoadSprite(selectedSlot);
            
            commandHighlighted = COMMAND_LOAD;
            commandHighlightedError = !success;
            commandHighlightedTimer = COMMAND_HIGHLIGHT_TIME;

        }        
        else if(isKeyDown(KEY_F3))
        {
            int success;
            
            success = trySaveSprite(selectedSlot);
            
            commandHighlighted = COMMAND_SAVE;
            commandHighlightedError = !success;
            commandHighlightedTimer = COMMAND_HIGHLIGHT_TIME;
        }        
        
        int mouseLeftPressed = isKeyPressed(MOUSE_LEFT) ? 1 : 0;
        int mouseRightPressed = isKeyPressed(MOUSE_RIGHT) ? 1 : 0;
        
        int cursorWindowX;
        int cursorWindowY;
        getMousePosition(&cursorWindowX, &cursorWindowY);
        
        cursorCellX = cursorWindowX / fontWidth;
        cursorCellY = cursorWindowY / fontHeight;
        
        if(mouseLeftPressed) { cursorChar = 'X'; }
        else { cursorChar = 'O'; }
        

        if(isInsideRect(cursorCellX, cursorCellY,
            ASCII_PALETTE_POSITION_X, ASCII_PALETTE_POSITION_Y,
            ASCII_PALETTE_WIDTH, ASCII_PALETTE_HEIGHT))
        {
            if(mouseLeftPressed)
            {
                selectedAsciiLevel = (cursorCellY - ASCII_PALETTE_POSITION_Y) * ASCII_PALETTE_WIDTH + (cursorCellX - ASCII_PALETTE_POSITION_X);
            }
        }
        else if(isInsideRect(cursorCellX, cursorCellY,
                        COLOR_PALETTE2_POSITION_X, COLOR_PALETTE2_POSITION_Y,
                        COLOR_PALETTE2_WIDTH, COLOR_PALETTE2_HEIGHT))
        {
            if(mouseLeftPressed)
            {
                selectedBlueLevel = cursorCellX - COLOR_PALETTE2_POSITION_X;
            }
        }
        else if(isInsideRect(cursorCellX, cursorCellY,
                        COLOR_PALETTE1_POSITION_X, COLOR_PALETTE1_POSITION_Y,
                        COLOR_PALETTE1_WIDTH, COLOR_PALETTE1_HEIGHT))
        {
            if(mouseLeftPressed)
            {            
                selectedRedLevel = cursorCellX - COLOR_PALETTE1_POSITION_X;
                selectedGreenLevel = cursorCellY - COLOR_PALETTE1_POSITION_Y;
            }            
        }
        else if(isInsideRect(cursorCellX, cursorCellY,
                        drawAreaPosX, drawAreaPosY,
                        drawAreaWidth, drawAreaHeight))
        {
            if(mouseLeftPressed || mouseRightPressed)
            {
                int s = selectedSlot;
                int x = cursorCellX - drawAreaPosX;
                int y = cursorCellY - drawAreaPosY;
                
                int canChangeCharacter = (selectedMode == MODE_ALL || selectedMode == MODE_CHAR);
                int canChangeColor = (selectedMode == MODE_ALL || selectedMode == MODE_COLOR);

                if(mouseLeftPressed)
                {  
                    if(selectedTool == TOOL_DRAW)
                    {
                        int r = (int)(selectedRedLevel * (255.0f / COLOR_PALETTE1_WIDTH));
                        int g = (int)(selectedGreenLevel * (255.0f / COLOR_PALETTE1_HEIGHT));
                        int b = (int)(selectedBlueLevel * (255.0f / COLOR_PALETTE2_WIDTH));
                        
                        char c = ASCII_FIRST + selectedAsciiLevel;                
                        
                        if(canChangeColor) { sprite[s][y][x].color = MAKE_COLOR(r, g, b); }
                        if(canChangeCharacter) { sprite[s][y][x].character = c; }
                    }
                    else // selectedTool == TOOL_PICK
                    {
                        int color = sprite[s][y][x].color;
                        int c = sprite[s][y][x].character;
                        
                        if(canChangeCharacter) { selectedAsciiLevel = c - ASCII_FIRST; }
                        if(canChangeColor)
                        {   selectedRedLevel = (int)(GET_COLOR_R(color) / (255.0f / COLOR_PALETTE1_WIDTH));
                            selectedGreenLevel = (int)(GET_COLOR_G(color) / (255.0f / COLOR_PALETTE1_HEIGHT));
                            selectedBlueLevel = (int)(GET_COLOR_B(color) / (255.0f / COLOR_PALETTE2_WIDTH));
                        }
                        
                        
                    }
                }
                else
                {
                    if(canChangeColor) { sprite[s][y][x].color = MAKE_COLOR(0, 0, 0); }
                    if(canChangeCharacter) { sprite[s][y][x].character = ' '; }
                    
                }
            }

        } 
        else if(isInsideRect(cursorCellX, cursorCellY,
                            SLOTS_POSITION_X, SLOTS_POSITION_Y,
                            SLOTS_WIDTH, SLOTS_HEIGHT))
        {
            if(mouseLeftPressed)
            {            
                selectedSlot = (cursorCellX - SLOTS_POSITION_X) / 2;
            }            
        }        
        else if(isInsideRect(cursorCellX, cursorCellY,
                            TOOLS_POSITION_X, TOOLS_POSITION_Y,
                            TOOLS_WIDTH, TOOLS_HEIGHT))
        {
            if(mouseLeftPressed)
            {            
                selectedTool = cursorCellY - TOOLS_POSITION_Y;
            }            
        }
        else if(isInsideRect(cursorCellX, cursorCellY,
                        MODES_POSITION_X, MODES_POSITION_Y,
                        MODES_WIDTH, MODES_HEIGHT))
        {
            if(mouseLeftPressed)
            {            
                selectedMode = cursorCellY - MODES_POSITION_Y;
            }            
        }        

        if(commandHighlightedTimer - (1.0f / SCREEN_FPS) >= 0) { commandHighlightedTimer -= (1.0f / SCREEN_FPS); }
        else
        {
            commandHighlighted = 0;
            commandHighlightedError = 0;
            commandHighlightedTimer = 0;
        }
        
        clearScreen();
        
        drawUI();
        
        setScreenCell(cursorCellX, cursorCellY, cursorColor, cursorChar);
        
        showScreen();
        
        wait((int)(1.0f / SCREEN_FPS * 1000));
    }
    
}