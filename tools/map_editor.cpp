#include "map_editor.hpp"
#include "editor_utils.hpp"
#include "map.hpp"

#define CELLS_WIDTH 70
#define CELLS_HEIGHT 40

#define COLOR_OPAQUE MAKE_COLOR(255,0,0)

#define MAP_FILE_PATTERN "map%02d.map"

#define ITEM_TYPE_COUNT 3

#define ITEM_TYPE_WIDTH 10
#define ITEM_TYPE_HEIGHT (ITEM_TYPE_COUNT / ITEM_TYPE_WIDTH + 1)

#define ITEM_TYPE_POSITION_X (CELLS_WIDTH - ITEM_TYPE_WIDTH - 2)
#define ITEM_TYPE_POSITION_Y 4

#define ITEM_PARAM_COUNT 10

#define ITEM_PARAM_WIDTH ITEM_PARAM_COUNT
#define ITEM_PARAM_HEIGHT 1

#define ITEM_PARAM_POSITION_X (CELLS_WIDTH - ITEM_PARAM_WIDTH - 2)
#define ITEM_PARAM_POSITION_Y (ITEM_TYPE_POSITION_Y + ITEM_TYPE_HEIGHT + 2 + 2)

#define ITEM_DESC_POSITION_X (CELLS_WIDTH - ITEM_DESC_WIDTH - 2)
#define ITEM_DESC_POSITION_Y (ITEM_PARAM_POSITION_Y + ITEM_PARAM_HEIGHT + 2 + 2)

#define ITEM_DESC_LENGTH 10

#define ITEM_DESC_WIDTH ITEM_DESC_LENGTH
#define ITEM_DESC_HEIGHT 2

#define TOOL_DRAW 0
#define TOOL_PICK 1
#define TOOL_FILL 2

#define TOOLS_COUNT 3

#define TOOLS_WIDTH 10
#define TOOLS_HEIGHT TOOLS_COUNT
#define TOOLS_POSITION_X 3
#define TOOLS_POSITION_Y 4

#define MODE_ALL   0
#define MODE_CHAR  1
#define MODE_COLOR 2
#define MODE_OPACITY 3

#define COMMAND_NEW 0
#define COMMAND_LOAD 1
#define COMMAND_SAVE 2
#define COMMAND_EXIT 3

#define COMMAND_HIGHLIGHT_TIME (0.5f)

#define COMMANDS_COUNT 4

#define COMMANDS_WIDTH 10
#define COMMANDS_HEIGHT COMMANDS_COUNT
#define COMMANDS_POSITION_X 3
#define COMMANDS_POSITION_Y (TOOLS_POSITION_Y + TOOLS_HEIGHT + 5)

#define ABOUT_WIDTH 10
#define ABOUT_HEIGHT 5
#define ABOUT_POSITION_X 3
#define ABOUT_POSITION_Y (CELLS_HEIGHT - ABOUT_HEIGHT - 1)

#define SLOTS_COUNT 16
#define SLOTS_WIDTH (SLOTS_COUNT * 2)
#define SLOTS_HEIGHT 1
#define SLOTS_POSITION_X (TOOLS_POSITION_X + TOOLS_WIDTH + 1 + 5)
#define SLOTS_POSITION_Y (CELLS_HEIGHT - SLOTS_HEIGHT - 1)

char itemTypeCharacters[ITEM_TYPE_COUNT] =
{ '.',
  '#',
  '@'
};

char itemTypeDescriptions[ITEM_TYPE_COUNT][ITEM_DESC_LENGTH + 1]
{
    " FLOOR",
    " WALL",
    " PLAYER"
};

char itemParamDescriptions[ITEM_TYPE_COUNT][ITEM_PARAM_COUNT][ITEM_DESC_LENGTH + 1]
{
    { " ", // 0
      " ", // 1
      " ", // 2
      " ", // 3
      " ", // 4
      " ", // 5
      " ", // 6
      " ", // 7
      " ", // 8
      " "  // 9
    },
    { " SMILEY", // 0
      " HITLER", // 1
      " ", // 2
      " ", // 3
      " ", // 4
      " ", // 5
      " ", // 6
      " ", // 7
      " ", // 8
      " "  // 9
    },
    { " NORTH", // 0
      " EAST",  // 1
      " SOUTH", // 2
      " WEST",  // 3
      " ", // 4
      " ", // 5
      " ", // 6
      " ", // 7
      " ", // 8
      " "  // 9
    }
};


int itemParamColors[ITEM_PARAM_COUNT]
{
    MAKE_COLOR(231,72,86),
    MAKE_COLOR(19,161,14 ),
    MAKE_COLOR(58,150,221),
    MAKE_COLOR(197,15,31),
    MAKE_COLOR(136,23,152),
    MAKE_COLOR(193,156,0),
    MAKE_COLOR(59,120,255),
    MAKE_COLOR(22,198,12),
    MAKE_COLOR(97,214,214),
    MAKE_COLOR(0,55,218 )
    
};



char versionString[VERSION_STRING_SIZE];

char cursorChar;
int cursorColor;
int cursorCellX;
int cursorCellY;

int selectedTool;

int selectedSlot;
int selectedItemType;
int selectedItemParam;

int drawAreaPosX = TOOLS_POSITION_X + TOOLS_WIDTH + 1 + 5;
int drawAreaPosY = 4;
int drawAreaWidth = 32;
int drawAreaHeight = 32;

char workPath[MAX_PATH_LENGTH];

int** mapCells[SLOTS_COUNT];
int spritePivots[SLOTS_COUNT * 2];
int spriteBounds[SLOTS_COUNT * 4];


float commandHighlightedTimer;
int commandHighlighted;
int commandHighlightedError;



void drawUI()
{
    drawWindow(ITEM_TYPE_POSITION_X - 1, ITEM_TYPE_POSITION_Y - 3, ITEM_TYPE_WIDTH + 2, ITEM_TYPE_HEIGHT + 4, "TYPE", MAKE_COLOR(255, 255, 0));
    
    drawWindow(ITEM_PARAM_POSITION_X - 1, ITEM_PARAM_POSITION_Y - 3, ITEM_PARAM_WIDTH + 2, ITEM_PARAM_HEIGHT + 4, "PARAM", MAKE_COLOR(255, 255, 0));    

    drawWindow(ITEM_DESC_POSITION_X - 1, ITEM_DESC_POSITION_Y - 3, ITEM_DESC_WIDTH + 2, ITEM_DESC_HEIGHT + 4, "DESC", MAKE_COLOR(255, 255, 0));    
    
    drawWindow(drawAreaPosX - 1, drawAreaPosY - 3, drawAreaWidth + 2, drawAreaHeight + 4, "MAP", MAKE_COLOR(255, 255, 0)); 

    drawWindow(SLOTS_POSITION_X - 1, SLOTS_POSITION_Y - 1, SLOTS_WIDTH + 2, SLOTS_HEIGHT + 2, NULL, MAKE_COLOR(255, 255, 0));
    
    drawWindow(TOOLS_POSITION_X - 1, TOOLS_POSITION_Y - 3, TOOLS_WIDTH + 2, TOOLS_HEIGHT + 4, "TOOLS", MAKE_COLOR(255, 255, 0));    

    drawWindow(COMMANDS_POSITION_X - 1, COMMANDS_POSITION_Y - 3, COMMANDS_WIDTH + 2, COMMANDS_HEIGHT + 4, "COMMAND", MAKE_COLOR(255, 255, 0)); 
    
    drawWindow(ABOUT_POSITION_X - 1, ABOUT_POSITION_Y - 1, ABOUT_WIDTH + 2, ABOUT_HEIGHT + 2, NULL, COLOR_UNSELECTED);     
    
    char itemType = 0;
    
    while(itemType < ITEM_TYPE_COUNT)
    {
        int color;
        if(itemType == selectedItemType)
        { color = COLOR_SELECTED; }
        else { color = COLOR_UNSELECTED; }
        
        setScreenCell(ITEM_TYPE_POSITION_X + itemType % ITEM_TYPE_WIDTH,
                     ITEM_TYPE_POSITION_Y + itemType / ITEM_TYPE_WIDTH, color, itemTypeCharacters[itemType]);
        
        itemType++;
    }
    
    
    for(int x = 0; x < ITEM_PARAM_WIDTH; x++)
    {
        int color;
        
        if(selectedItemParam  == x) { color = COLOR_SELECTED; }
        else { color = itemParamColors[x]; }            
        
        setScreenCell(ITEM_PARAM_POSITION_X + x, ITEM_PARAM_POSITION_Y, color, '0' + x);
    }
    
        
    drawString(COLOR_SELECTED, itemTypeDescriptions[selectedItemType], ITEM_DESC_POSITION_X, ITEM_DESC_POSITION_Y);
    drawString(COLOR_SELECTED, itemParamDescriptions[selectedItemType][selectedItemParam], ITEM_DESC_POSITION_X, ITEM_DESC_POSITION_Y + 1);


    for(int y = 0; y < drawAreaHeight; y++)
    {
        for(int x = 0; x < drawAreaWidth; x ++)
        {
            int type = MAP_CELL_TYPE(mapCells[selectedSlot][y][x]);
            int param = MAP_CELL_PARAM(mapCells[selectedSlot][y][x]);
                        
            setScreenCell(drawAreaPosX + x, drawAreaPosY + y, itemParamColors[param], itemTypeCharacters[type]);
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
    drawString(selectedTool == TOOL_FILL ? COLOR_SELECTED : COLOR_UNSELECTED, " FILL", TOOLS_POSITION_X, TOOLS_POSITION_Y + 2);

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
    
    drawString(COLOR_UNSELECTED, "   MAP  ", ABOUT_POSITION_X, ABOUT_POSITION_Y + 0);
    drawString(COLOR_UNSELECTED, "  EDITOR  ", ABOUT_POSITION_X, ABOUT_POSITION_Y + 1);
    drawString(COLOR_UNSELECTED, versionString, ABOUT_POSITION_X, ABOUT_POSITION_Y + 2);
    drawString(COLOR_UNSELECTED, "    BY    ", ABOUT_POSITION_X, ABOUT_POSITION_Y + 3);
    drawString(COLOR_UNSELECTED, " JM SOLIS ", ABOUT_POSITION_X, ABOUT_POSITION_Y + 4);

}

int tryLoadMap(int slot)
{
    int success;
    
    char fileName[MAX_PATH_LENGTH];
    sprintf(fileName, MAP_FILE_PATTERN, slot);
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
                fread(&mapCells[slot][y][x], sizeof(int), 1, file);
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

int trySaveMap(int slot)
{
    int success;
    
    char fileName[MAX_PATH_LENGTH];
    sprintf(fileName, MAP_FILE_PATTERN, slot);
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
                fwrite(&mapCells[slot][y][x], sizeof(int), 1, file);
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

void fillMap(int cellX, int cellY, int replacingType, int replacingParam)
{
    if(cellX < 0 || cellX > drawAreaWidth - 1 || cellY < 0 || cellY > drawAreaHeight - 1)
    {
        return;
    }
    else
    {
        int existingType = MAP_CELL_TYPE(mapCells[selectedSlot][cellY][cellX]);
        int existingParam = MAP_CELL_PARAM(mapCells[selectedSlot][cellY][cellX]);

        if(existingType != replacingType ||
           existingType == replacingType && existingParam != replacingParam ||
           (existingType == selectedItemType && existingParam == selectedItemParam))
        {
            return;
        }
        else
        {
            mapCells[selectedSlot][cellY][cellX] = MAKE_MAP_CELL(selectedItemType, selectedItemParam);
            
            fillMap(cellX + 1, cellY, replacingType, replacingParam);
            fillMap(cellX - 1, cellY, replacingType, replacingParam);
            fillMap(cellX, cellY - 1, replacingType, replacingParam);
            fillMap(cellX, cellY + 1, replacingType, replacingParam);        
        }
    }
        
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
    
    setScreenTitle("Map editor");
    
    resizeScreen(CELLS_WIDTH * fontWidth, CELLS_HEIGHT * fontHeight);
    
    int quit = 0;
    
    cursorChar = 'O';
    cursorColor = MAKE_COLOR(255, 255, 255);
    
    selectedTool = TOOL_DRAW;
    
    selectedSlot = 0;
    selectedItemType = 0;
    selectedItemParam = 0;
    
    commandHighlighted = 0;
    commandHighlightedTimer = 0;
    commandHighlightedError = 0;
    
    for(int s = 0; s < SLOTS_COUNT; s++)
    {
        mapCells[s] = (int**)malloc(sizeof(int*) * drawAreaHeight);
        for(int i = 0; i < drawAreaHeight; i ++)
        { 
            mapCells[s][i] = (int*)malloc(sizeof(int) * drawAreaWidth);
            
            for(int j = 0; j < drawAreaWidth; j++)
            {
                mapCells[s][i][j] = 0;
            }
        }
        
    }
    
    for(int i = 0; i < SLOTS_COUNT; i++)
    {
        tryLoadMap(i);
    }
        
    while(!quit)
    {
        if(isKeyDown(KEY_ESC))
        {
            quit = 1;
        }
        else if(isKeyDown(KEY_F1))
        {
            for(int y = 0; y < drawAreaHeight; y++)
            {
                for(int x = 0; x < drawAreaWidth; x++)
                {
                    mapCells[selectedSlot][y][x] = 0;
                }
            }
            
            commandHighlighted = COMMAND_NEW;
            commandHighlightedError = 0;
            commandHighlightedTimer = COMMAND_HIGHLIGHT_TIME;
            
            
        }
        else if(isKeyDown(KEY_F2))
        {
            int success;
            
            success = tryLoadMap(selectedSlot);
            
            commandHighlighted = COMMAND_LOAD;
            commandHighlightedError = !success;
            commandHighlightedTimer = COMMAND_HIGHLIGHT_TIME;

        }        
        else if(isKeyDown(KEY_F3))
        {
            int success;
            
            success = trySaveMap(selectedSlot);
            
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
            ITEM_TYPE_POSITION_X, ITEM_TYPE_POSITION_Y,
            ITEM_TYPE_WIDTH, ITEM_TYPE_HEIGHT))
        {
            if(mouseLeftPressed)
            {
                selectedItemType = (cursorCellY - ITEM_TYPE_POSITION_Y) * ITEM_TYPE_WIDTH + (cursorCellX - ITEM_TYPE_POSITION_X);
            }
        }
        else if(isInsideRect(cursorCellX, cursorCellY,
                        ITEM_PARAM_POSITION_X, ITEM_PARAM_POSITION_Y,
                        ITEM_PARAM_WIDTH, ITEM_PARAM_HEIGHT))
        {
            if(mouseLeftPressed)
            {            
                selectedItemParam = cursorCellX - ITEM_PARAM_POSITION_X;
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
                
                if(selectedTool == TOOL_DRAW)
                {
                    if(mouseLeftPressed)
                    {  
                        mapCells[s][y][x] = MAKE_MAP_CELL(selectedItemType, selectedItemParam);
                    }
                    else
                    {
                        mapCells[s][y][x] = 0;
                    }
                }
                else if(selectedTool == TOOL_PICK)
                {
                    if(mouseLeftPressed)
                    {                      
                        selectedItemType = MAP_CELL_TYPE(mapCells[s][y][x]);
                        selectedItemParam = MAP_CELL_PARAM(mapCells[s][y][x]);
                    }
                }
                else if(selectedTool == TOOL_FILL)
                {
                    if(mouseLeftPressed)
                    {
                        int type = MAP_CELL_TYPE(mapCells[s][y][x]);
                        int param = MAP_CELL_PARAM(mapCells[s][y][x]);

                        fillMap(x, y, type, param);
                    }
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