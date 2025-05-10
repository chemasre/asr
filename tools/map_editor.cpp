#include "map_editor.hpp"
#include "editor_utils.hpp"
#include "map.hpp"

#define CELLS_WIDTH 74
#define CELLS_HEIGHT 40

#define COLOR_OPAQUE MAKE_COLOR(255,0,0)

#define MAP_FILE_PATTERN "map%02d.map"

#define ITEM_TYPE_COUNT 4

#define ITEM_TYPE_WIDTH 16
#define ITEM_TYPE_HEIGHT (ITEM_TYPE_COUNT / ITEM_TYPE_WIDTH + 1)

#define ITEM_TYPE_POSITION_X (CELLS_WIDTH - ITEM_TYPE_WIDTH - 2)
#define ITEM_TYPE_POSITION_Y 4

#define ITEM_PARAMS_COUNT 2

#define ITEM_PARAMS_VALUES_COUNT 16

#define ITEM_PARAMS_WIDTH ITEM_PARAMS_VALUES_COUNT
#define ITEM_PARAMS_HEIGHT (ITEM_PARAMS_COUNT * 2 - 1)

#define ITEM_PARAMS_POSITION_X (CELLS_WIDTH - ITEM_PARAMS_WIDTH - 2)
#define ITEM_PARAMS_POSITION_Y (ITEM_TYPE_POSITION_Y + ITEM_TYPE_HEIGHT + 2 + 2)

#define ITEM_DESC_POSITION_X (CELLS_WIDTH - ITEM_DESC_WIDTH - 2)
#define ITEM_DESC_POSITION_Y (ITEM_PARAMS_POSITION_Y + ITEM_PARAMS_HEIGHT + 2 + 2)

#define DESC_LENGTH 16

#define ITEM_DESC_WIDTH DESC_LENGTH
#define ITEM_DESC_HEIGHT (ITEM_PARAMS_COUNT + 1)

#define ITEM_PARAM_DESC_MAXPATTERNS 100

// Bottom aligned

#define MAP_PARAMS_VALUES_COUNT 16
#define MAP_PARAMS_COUNT 4

#define MAP_DESC_WIDTH DESC_LENGTH
#define MAP_DESC_HEIGHT MAP_PARAMS_COUNT

#define MAP_DESC_POSITION_X (CELLS_WIDTH - MAP_DESC_WIDTH - 2)
#define MAP_DESC_POSITION_Y (CELLS_HEIGHT - MAP_DESC_HEIGHT - 1)

#define MAP_PARAMS_WIDTH MAP_PARAMS_VALUES_COUNT
#define MAP_PARAMS_HEIGHT (MAP_PARAMS_COUNT * 2 - 1)

#define MAP_PARAMS_POSITION_X (CELLS_WIDTH - MAP_PARAMS_WIDTH - 2)
#define MAP_PARAMS_POSITION_Y (MAP_DESC_POSITION_Y - MAP_PARAMS_HEIGHT - 4)

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

#define COMMANDS1_COUNT 4

#define COMMANDS1_WIDTH 10
#define COMMANDS1_HEIGHT COMMANDS1_COUNT
#define COMMANDS1_POSITION_X 3
#define COMMANDS1_POSITION_Y (TOOLS_POSITION_Y + TOOLS_HEIGHT + 5)

#define SLOTS_COUNT 16
#define SLOTS_WIDTH (SLOTS_COUNT * 2)
#define SLOTS_HEIGHT 1
#define SLOTS_POSITION_X (TOOLS_POSITION_X + TOOLS_WIDTH + 1 + 5)
#define SLOTS_POSITION_Y (CELLS_HEIGHT - SLOTS_HEIGHT - 1)

char itemTypeCharacters[ITEM_TYPE_COUNT] =
{ '.',
  '#',
  '@',
  'T'
  
};

char itemTypeDescriptions[ITEM_TYPE_COUNT][DESC_LENGTH + 1]
{
    " FLOOR",
    " WALL",
    " PLAYER",
    " TWINS"
};

struct ItemParamDescriptionPattern
{
	int param;
	int value;
	char description[DESC_LENGTH + 1];
};

ItemParamDescriptionPattern itemParamDescriptionPatterns[ITEM_TYPE_COUNT][ITEM_PARAM_DESC_MAXPATTERNS] =
{
	// TYPE 0
	
    {
	  { 0, 0, " EMPTY"},
	  { 0, 1, " SOFAS"},
	  { 0, 2, " BINS"},
	  { 0, 3, " POTS"},
	  { 0, 4, " TREEPOTS"},
	  { 0, 5, " CANDLELAMP"},
	  { 0, 6, " ROUNDLAMP"},
	  
	  { 1, 0, " DEFAULT"},
	  { 1, 1, " HORIZONTAL"},	  
	  { 1, 2, " VERTICAL"},	  
	  { -1, 0, "" }
    },

	// TYPE 1

    {
	  { 0, 0, " ROOM" },
      { 0, 1, " ELEVATOR" },
      { 0, 2, " CORRIDOR" },
	  { -1, 0, "" }
    },
	
	// TYPE 2

    { 
	  { 0, 0, " NORTH"},
	  { 0, 1, " EAST" },
	  { 0, 2, " SOUTH"},
	  { 0, 3, " WEST" },
	  { -1, 0, "" }
    },

	// TYPE 3

    { 
	  { -1, 0, "" }
    }

};

char mapParamsDescriptions[MAP_PARAMS_COUNT][MAP_PARAMS_VALUES_COUNT][DESC_LENGTH + 1]
{
    { " AMBIENT 0.00", // 0
      " AMBIENT 0.06", // 1
      " AMBIENT 0.13", // 2
      " AMBIENT 0.20", // 3
      " AMBIENT 0.26", // 4
      " AMBIENT 0.33", // 5
      " AMBIENT 0.34", // 6
      " AMBIENT 0.46", // 7
      " AMBIENT 0.53", // 8
      " AMBIENT 0.60", // 9
      " AMBIENT 0.66", // 10
      " AMBIENT 0.73", // 11
      " AMBIENT 0.80", // 12
      " AMBIENT 0.86", // 13
      " AMBIENT 0.93", // 14
      " AMBIENT 1.00"  // 15
    },
    { " SUN DIR   0.0", // 0
      " SUN DIR  22.5", // 1
      " SUN DIR  45.0", // 2
      " SUN DIR  67.5", // 3
      " SUN DIR  90.0", // 4
      " SUN DIR 112.5", // 5
      " SUN DIR 135.0", // 6
      " SUN DIR 157.5", // 7
      " SUN DIR 180.0", // 8
      " SUN DIR 202.5", // 9
      " SUN DIR 225.0", // 10
      " SUN DIR 247.5", // 11
      " SUN DIR 270.0", // 12
      " SUN DIR 292.5", // 13
      " SUN DIR 315.0", // 14
      " SUN DIR 337.5"  // 15
    },
    { " SUN 0.00", // 0
      " SUN 0.06", // 1
      " SUN 0.13", // 2
      " SUN 0.20", // 3
      " SUN 0.26", // 4
      " SUN 0.33", // 5
      " SUN 0.34", // 6
      " SUN 0.46", // 7
      " SUN 0.53", // 8
      " SUN 0.60", // 9
      " SUN 0.66", // 10
      " SUN 0.73", // 11
      " SUN 0.80", // 12
      " SUN 0.86", // 13
      " SUN 0.93", // 14
      " SUN 1.00"  // 15
    },
    { "", // 0
      "", // 1
      "", // 2
      "", // 3
      "", // 4
      "", // 5
      "", // 6
      "", // 7
      "", // 8
      "", // 9
      "", // 10
      "", // 11
      "", // 12
      "", // 13
      "", // 14
      ""  // 15
    }
    
};


int itemParamColors[ITEM_PARAMS_VALUES_COUNT]
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
    MAKE_COLOR(0,55,218 ),
    MAKE_COLOR(150,15,31),
    MAKE_COLOR(150,23,152),
    MAKE_COLOR(150,156,0),
    MAKE_COLOR(150,120,255),
    MAKE_COLOR(150,198,12),
    MAKE_COLOR(150,214,214)
    
};

char cursorChar;
int cursorColor;
int cursorCellX;
int cursorCellY;

int selectedTool;

int selectedSlot;
int selectedItemType;
int selectedItemParams[ITEM_PARAMS_COUNT];

int drawAreaPosX = TOOLS_POSITION_X + TOOLS_WIDTH + 1 + 5;
int drawAreaPosY = 4;
int drawAreaWidth = 32;
int drawAreaHeight = 32;

char workPath[MAX_PATH_LENGTH];

int** _mapCells[SLOTS_COUNT];
int* _mapParams[SLOTS_COUNT];


float commandHighlightedTimer;
int commandHighlighted;
int commandHighlightedError;

void drawUI()
{
    drawWindow(ITEM_TYPE_POSITION_X - 1, ITEM_TYPE_POSITION_Y - 3, ITEM_TYPE_WIDTH + 2, ITEM_TYPE_HEIGHT + 4, "TYPE", MAKE_COLOR(255, 255, 0));
    
    drawWindow(ITEM_PARAMS_POSITION_X - 1, ITEM_PARAMS_POSITION_Y - 3, ITEM_PARAMS_WIDTH + 2, ITEM_PARAMS_HEIGHT + 4, "ITEM PARAMS", MAKE_COLOR(255, 255, 0));    

    drawWindow(ITEM_DESC_POSITION_X - 1, ITEM_DESC_POSITION_Y - 3, ITEM_DESC_WIDTH + 2, ITEM_DESC_HEIGHT + 4, "DESC", MAKE_COLOR(255, 255, 0));    
    for(int i = 0; i < ITEM_PARAMS_COUNT; i ++)
    {
        fillScreenArea(ITEM_PARAMS_POSITION_X, ITEM_PARAMS_POSITION_Y + i * 2 + 1, ITEM_PARAMS_WIDTH, 1, MAKE_COLOR(255, 255, 0), '-');
    }

    
    drawWindow(MAP_PARAMS_POSITION_X - 1, MAP_PARAMS_POSITION_Y - 3, MAP_PARAMS_WIDTH + 2, MAP_PARAMS_HEIGHT + 4, "MAP PARAMS", MAKE_COLOR(255, 255, 0));
    
    drawWindow(MAP_DESC_POSITION_X - 1, MAP_DESC_POSITION_Y - 3, MAP_DESC_WIDTH + 2, MAP_DESC_HEIGHT + 4, "DESC", MAKE_COLOR(255, 255, 0));    

    for(int i = 0; i < MAP_PARAMS_COUNT; i ++)
    {
        fillScreenArea(MAP_PARAMS_POSITION_X, MAP_PARAMS_POSITION_Y + i * 2 + 1, MAP_PARAMS_WIDTH, 1, MAKE_COLOR(255, 255, 0), '-');
    }

    drawWindow(drawAreaPosX - 1, drawAreaPosY - 3, drawAreaWidth + 2, drawAreaHeight + 4, "MAP", MAKE_COLOR(255, 255, 0)); 

    drawWindow(SLOTS_POSITION_X - 1, SLOTS_POSITION_Y - 1, SLOTS_WIDTH + 2, SLOTS_HEIGHT + 2, NULL, MAKE_COLOR(255, 255, 0));
    
    drawWindow(TOOLS_POSITION_X - 1, TOOLS_POSITION_Y - 3, TOOLS_WIDTH + 2, TOOLS_HEIGHT + 4, "TOOLS", MAKE_COLOR(255, 255, 0));    

    drawWindow(COMMANDS1_POSITION_X - 1, COMMANDS1_POSITION_Y - 3, COMMANDS1_WIDTH + 2, COMMANDS1_HEIGHT + 4, "COMMAND", MAKE_COLOR(255, 255, 0)); 
        
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
    
    
	for(int p = 0; p < ITEM_PARAMS_COUNT; p++)
	{
		for(int x = 0; x < ITEM_PARAMS_WIDTH; x++)
		{
			int color;
			
			if(selectedItemParams[p]  == x) { color = COLOR_SELECTED; }
			else { color = itemParamColors[x]; }            
			
			setScreenCell(ITEM_PARAMS_POSITION_X + x, ITEM_PARAMS_POSITION_Y + p * 2, color, x <= 9 ? '0' + x : 'A' + x - 10);
		}
	}
	
	int hasItemDescription = 0;
	
	if(selectedItemType < ITEM_TYPE_COUNT)
	{
		drawString(COLOR_SELECTED, itemTypeDescriptions[selectedItemType], ITEM_DESC_POSITION_X, ITEM_DESC_POSITION_Y);
		hasItemDescription = 1;
	}
		
	if(hasItemDescription)
	{
		int foundPrevious = 1;
		int p = 0;
		
		while(p < ITEM_PARAMS_COUNT && foundPrevious)
		{
			int i = 0;
			int found = 0;
			int foundIndex = 0;
			int none = 0;
			while(!(found || none) && i < ITEM_PARAM_DESC_MAXPATTERNS)
			{
				if(itemParamDescriptionPatterns[selectedItemType][i].param < 0) { none = 1; }
				else if(itemParamDescriptionPatterns[selectedItemType][i].param == p &&
						itemParamDescriptionPatterns[selectedItemType][i].value == selectedItemParams[p]) { found = 1; foundIndex = i; }
				else { i ++; }

			}
			
			if(found)
			{
				drawString(COLOR_SELECTED, itemParamDescriptionPatterns[selectedItemType][foundIndex].description, ITEM_DESC_POSITION_X, ITEM_DESC_POSITION_Y +1 +  p);
				
				p ++;
			}
			else
			{
				foundPrevious = 0;
			}
		}
	}
        
    for(int x = 0; x < MAP_PARAMS_WIDTH; x++)
    {
        for(int y = 0; y < MAP_PARAMS_COUNT; y ++)
        {
            int color;
            
            if(_mapParams[selectedSlot][y]  == x) { color = COLOR_SELECTED; }
            else { color = itemParamColors[x]; }            
            
            setScreenCell(MAP_PARAMS_POSITION_X + x, MAP_PARAMS_POSITION_Y + y * 2, color, x <= 9 ? '0' + x : 'A' + x - 10);
        }
    }  

    for(int y = 0; y < MAP_PARAMS_COUNT; y ++)
    {
        drawString(COLOR_SELECTED, mapParamsDescriptions[y][_mapParams[selectedSlot][y]], MAP_DESC_POSITION_X, MAP_DESC_POSITION_Y + y);        
    }


    for(int y = 0; y < drawAreaHeight; y++)
    {
        for(int x = 0; x < drawAreaWidth; x ++)
        {
            int type = MAP_CELL_TYPE(_mapCells[selectedSlot][y][x]);
            int param = MAP_CELL_PARAM(_mapCells[selectedSlot][y][x], 0);
                        
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
    
    drawString(COLOR_SELECTED, " F1", COMMANDS1_POSITION_X, COMMANDS1_POSITION_Y + 0);
    drawString(highlightNew ? highlightColor : COLOR_UNSELECTED, "NEW", COMMANDS1_POSITION_X + 5, COMMANDS1_POSITION_Y + 0);

    drawString(COLOR_SELECTED, " F2", COMMANDS1_POSITION_X, COMMANDS1_POSITION_Y + 1);
    drawString(highlightLoad ? highlightColor : COLOR_UNSELECTED, "LOAD", COMMANDS1_POSITION_X + 5, COMMANDS1_POSITION_Y + 1);

    drawString(COLOR_SELECTED, " F3", COMMANDS1_POSITION_X, COMMANDS1_POSITION_Y + 2);
    drawString(highlightSave ? highlightColor : COLOR_UNSELECTED, "SAVE", COMMANDS1_POSITION_X + 5, COMMANDS1_POSITION_Y + 2);

    drawString(COLOR_SELECTED, " ESC", COMMANDS1_POSITION_X, COMMANDS1_POSITION_Y + 3);
    drawString(COLOR_UNSELECTED, "EXIT", COMMANDS1_POSITION_X + 5, COMMANDS1_POSITION_Y + 3);
    
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
        int mapParamsCount = MAP_PARAMS_COUNT;
        
        fread(&mapParamsCount, sizeof(int), 1, file);
        
        for(int i = 0; i < mapParamsCount; i++)
        {
            fread(&_mapParams[slot][i], sizeof(int), 1, file);
        }
        
        fread(&drawAreaWidth, sizeof(int), 1, file);
        fread(&drawAreaHeight, sizeof(int), 1, file);
        
        for(int y = 0; y < drawAreaHeight; y++)
        {
            for(int x = 0; x < drawAreaWidth; x++)
            {
                fread(&_mapCells[slot][y][x], sizeof(int), 1, file);				
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
        int mapParamsCount = MAP_PARAMS_COUNT;
        
        fwrite(&mapParamsCount, sizeof(int), 1, file);
        
        for(int i = 0; i < mapParamsCount; i++)
        {
            fwrite(&_mapParams[slot][i], sizeof(int), 1, file);
        }
        
        fwrite(&drawAreaWidth, sizeof(int), 1, file);
        fwrite(&drawAreaHeight, sizeof(int), 1, file);
        
        for(int y = 0; y < drawAreaHeight; y++)
        {
            for(int x = 0; x < drawAreaWidth; x++)
            {
                fwrite(&_mapCells[slot][y][x], sizeof(int), 1, file);
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

void fillMap(int cellX, int cellY, int replacingType, int replacingParams[ITEM_PARAMS_COUNT])
{
    if(cellX < 0 || cellX > drawAreaWidth - 1 || cellY < 0 || cellY > drawAreaHeight - 1)
    {
        return;
    }
    else
    {
        int existingType = MAP_CELL_TYPE(_mapCells[selectedSlot][cellY][cellX]);
		
        int existingParams[ITEM_PARAMS_COUNT];

		for(int p = 0; p < ITEM_PARAMS_COUNT; p++)
		{
			existingParams[p] = MAP_CELL_PARAM(_mapCells[selectedSlot][cellY][cellX], p);
		}
		
		int sameParams = 1;
		int p = 0;
		while(sameParams && p < ITEM_PARAMS_COUNT) { sameParams = sameParams && (existingParams[p] == replacingParams[p]); p++; }
		

        if(existingType != replacingType ||
           existingType == replacingType && !sameParams ||
           (existingType == selectedItemType && sameParams))
        {
            return;
        }
        else
        {
            _mapCells[selectedSlot][cellY][cellX] = MAKE_MAP_CELL(selectedItemType);
			for(int p = 0; p < ITEM_PARAMS_COUNT; p++)
			{
				_mapCells[selectedSlot][cellY][cellX] |= MAKE_MAP_CELL_PARAM(p, selectedItemParams[p]);
			}
            
            fillMap(cellX + 1, cellY, replacingType, replacingParams);
            fillMap(cellX - 1, cellY, replacingType, replacingParams);
            fillMap(cellX, cellY - 1, replacingType, replacingParams);
            fillMap(cellX, cellY + 1, replacingType, replacingParams);        
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
    initUI();
    initMenu();
    
	char titleString[TITLE_STRING_SIZE];
	sprintf(titleString, TITLE_STRING_PATTERN, "Map Editor", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, "Jose M Solis");
    setScreenTitle(titleString);
    
    resizeScreen(CELLS_WIDTH * fontWidth, CELLS_HEIGHT * fontHeight);
    
    int quit = 0;
    
    cursorChar = 'O';
    cursorColor = MAKE_COLOR(255, 255, 255);
    
    selectedTool = TOOL_DRAW;
    
    selectedSlot = 0;
    selectedItemType = 0;
    for(int p = 0; p < ITEM_PARAMS_COUNT; p++) { selectedItemParams[p] = 0; }
    
    commandHighlighted = 0;
    commandHighlightedTimer = 0;
    commandHighlightedError = 0;
    
    for(int s = 0; s < SLOTS_COUNT; s++)
    {
        _mapCells[s] = (int**)malloc(sizeof(int*) * drawAreaHeight);
        for(int i = 0; i < drawAreaHeight; i ++)
        { 
            _mapCells[s][i] = (int*)malloc(sizeof(int) * drawAreaWidth);
            
            for(int j = 0; j < drawAreaWidth; j++)
            {
                _mapCells[s][i][j] = 0;
            }
        }
        
        _mapParams[s] = (int*)malloc(sizeof(int) * MAP_PARAMS_COUNT);
        
        for(int i = 0; i < MAP_PARAMS_COUNT; i++)
        {
            _mapParams[s][i] = 0;
        }
        
    }
    
    for(int i = 0; i < SLOTS_COUNT; i++)
    {
        tryLoadMap(i);
    }
        
    while(!quit)
    {
		if(!isKeyPressed(KEY_ALT))
		{
			if(isKeyPressed(KEY_0)) { selectedSlot = 0; }
			else if(isKeyPressed(KEY_1)) { selectedSlot = 1; }
			else if(isKeyPressed(KEY_2)) { selectedSlot = 2; }
			else if(isKeyPressed(KEY_3)) { selectedSlot = 3; }
			else if(isKeyPressed(KEY_4)) { selectedSlot = 4; }
			else if(isKeyPressed(KEY_5)) { selectedSlot = 5; }
			else if(isKeyPressed(KEY_6)) { selectedSlot = 6; }
			else if(isKeyPressed(KEY_7)) { selectedSlot = 7; }
			else if(isKeyPressed(KEY_8)) { selectedSlot = 8; }
			else if(isKeyPressed(KEY_9)) { selectedSlot = 9; }
			else if(isKeyPressed(KEY_A)) { selectedSlot = 10; }
			else if(isKeyPressed(KEY_B)) { selectedSlot = 11; }
			else if(isKeyPressed(KEY_C)) { selectedSlot = 12; }
			else if(isKeyPressed(KEY_D)) { selectedSlot = 13; }
			else if(isKeyPressed(KEY_E)) { selectedSlot = 14; }
			else if(isKeyPressed(KEY_F)) { selectedSlot = 15; }
		}
		
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
                    _mapCells[selectedSlot][y][x] = 0;
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
                        ITEM_PARAMS_POSITION_X, ITEM_PARAMS_POSITION_Y,
                        ITEM_PARAMS_WIDTH, ITEM_PARAMS_HEIGHT))
        {
            if(mouseLeftPressed)
            {            
				int line = (cursorCellY - ITEM_PARAMS_POSITION_Y);
				if(line % 2 == 0)
				{
					selectedItemParams[line / 2] = cursorCellX - ITEM_PARAMS_POSITION_X;
				}
            }            
        }
        else if(isInsideRect(cursorCellX, cursorCellY,
                MAP_PARAMS_POSITION_X, MAP_PARAMS_POSITION_Y,
                MAP_PARAMS_WIDTH, MAP_PARAMS_HEIGHT))
        {
            if(mouseLeftPressed)
            { 
                int line = (cursorCellY - MAP_PARAMS_POSITION_Y);                
                if(line % 2 == 0)
                {
                    _mapParams[selectedSlot][line / 2] = cursorCellX - MAP_PARAMS_POSITION_X;
                }
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
                        _mapCells[s][y][x] = MAKE_MAP_CELL(selectedItemType);

						for(int p = 0; p < ITEM_PARAMS_COUNT; p ++)
						{
							_mapCells[s][y][x] |= MAKE_MAP_CELL_PARAM(p, selectedItemParams[p]);
						}
                    }
                    else
                    {
                        _mapCells[s][y][x] = 0;
                    }
                }
                else if(selectedTool == TOOL_PICK)
                {
                    if(mouseLeftPressed)
                    {                      
                        selectedItemType = MAP_CELL_TYPE(_mapCells[s][y][x]);
						
						for(int p = 0; p < ITEM_PARAMS_COUNT; p++)
						{
							selectedItemParams[p] = MAP_CELL_PARAM(_mapCells[s][y][x], p);
						}
                    }
                }
                else if(selectedTool == TOOL_FILL)
                {
                    if(mouseLeftPressed)
                    {
                        int type = MAP_CELL_TYPE(_mapCells[s][y][x]);
                        int params[ITEM_PARAMS_COUNT];
						for(int p = 0; p < ITEM_PARAMS_COUNT; p ++)
						{
							params[p] = MAP_CELL_PARAM(_mapCells[s][y][x], p);
						}

                        fillMap(x, y, type, params);
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