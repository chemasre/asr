#include "sprite_editor.hpp"
#include "editor_utils.hpp"

#define CELLS_WIDTH 73
#define CELLS_HEIGHT 40

#define COLOR_OPAQUE MAKE_COLOR(255,0,0)
#define COLOR_BOUNDS MAKE_COLOR(0,255,255)

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

#define OPAQUE_POSITION_X (CELLS_WIDTH - OPAQUE_WIDTH - 2)
#define OPAQUE_POSITION_Y (COLOR_PALETTE2_POSITION_Y  + COLOR_PALETTE2_HEIGHT + 2 + 2)

#define OPAQUE_WIDTH 12
#define OPAQUE_HEIGHT 1

#define TOOL_SELECT 0
#define TOOL_DRAW 1
#define TOOL_PICK 2
#define TOOL_PIVOT 3
#define TOOL_BOUNDS 4

#define TOOLS_COUNT 5
#define TOOLS_WIDTH 12
#define TOOLS_HEIGHT TOOLS_COUNT
#define TOOLS_POSITION_X 3
#define TOOLS_POSITION_Y 4

#define MODE_ALL   0
#define MODE_CHAR  1
#define MODE_COLOR 2
#define MODE_OPACITY 3

#define MODES_COUNT 4
#define MODES_WIDTH 12
#define MODES_HEIGHT MODES_COUNT
#define MODES_POSITION_X 3
#define MODES_POSITION_Y (TOOLS_POSITION_Y + TOOLS_HEIGHT + 5)

#define COMMAND_NEW 0
#define COMMAND_LOAD 1
#define COMMAND_SAVE 2
#define COMMAND_EXIT 3
#define COMMAND_COPY 4
#define COMMAND_CUT 5
#define COMMAND_PASTE 6
#define COMMAND_CLEAR 7
#define COMMAND_MIRROR_X 8
#define COMMAND_MIRROR_Y 9

#define COMMAND_HIGHLIGHT_TIME (0.5f)

#define COMMANDS1_COUNT 4

#define COMMANDS1_WIDTH 12
#define COMMANDS1_HEIGHT COMMANDS1_COUNT
#define COMMANDS1_POSITION_X 3
#define COMMANDS1_POSITION_Y (MODES_POSITION_Y + MODES_HEIGHT + 5)

#define COMMANDS2_COUNT 6

#define COMMANDS2_WIDTH 12
#define COMMANDS2_HEIGHT COMMANDS2_COUNT
#define COMMANDS2_POSITION_X 3
#define COMMANDS2_POSITION_Y (COMMANDS1_POSITION_Y + COMMANDS1_HEIGHT + 5)

#define SLOTS_COUNT 16
#define SLOTS_WIDTH (SLOTS_COUNT * 2)
#define SLOTS_HEIGHT 1
#define SLOTS_POSITION_X (TOOLS_POSITION_X + TOOLS_WIDTH + 1 + 5)
#define SLOTS_POSITION_Y (CELLS_HEIGHT - SLOTS_HEIGHT - 1)

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
int selectedOpacity;

int selectionPosX;
int selectionPosY;
int selectionWidth;
int selectionHeight;

int drawAreaPosX = TOOLS_POSITION_X + TOOLS_WIDTH + 1 + 5;
int drawAreaPosY = 4;
int drawAreaWidth = 32;
int drawAreaHeight = 32;

char workPath[MAX_PATH_LENGTH];

SpriteCell** spriteCells[SLOTS_COUNT];
int spritePivots[SLOTS_COUNT * 2];
int spriteBounds[SLOTS_COUNT * 4];

SpriteCell** copyBuffer;
int copyBufferPosX;
int copyBufferPosY;
int copyBufferWidth;
int copyBufferHeight;
int copyBufferHasColor;
int copyBufferHasCharacters;
int copyBufferHasOpacity;

float commandHighlightedTimer;
int commandHighlighted;
int commandHighlightedError;

void drawUI()
{
    drawWindow(ASCII_PALETTE_POSITION_X - 1, ASCII_PALETTE_POSITION_Y - 3, ASCII_PALETTE_WIDTH + 2, ASCII_PALETTE_HEIGHT + 4, "ASCII", MAKE_COLOR(255, 255, 0));
    
    drawWindow(COLOR_PALETTE1_POSITION_X - 1, COLOR_PALETTE1_POSITION_Y - 3, COLOR_PALETTE1_WIDTH + 2, COLOR_PALETTE1_HEIGHT + 4, "COLOR", MAKE_COLOR(255, 255, 0));    

    drawWindow(COLOR_PALETTE2_POSITION_X - 1, COLOR_PALETTE2_POSITION_Y - 3, COLOR_PALETTE2_WIDTH + 2, COLOR_PALETTE2_HEIGHT + 4, "COLOR", MAKE_COLOR(255, 255, 0));    
    
    drawWindow(OPAQUE_POSITION_X - 1, OPAQUE_POSITION_Y - 3, OPAQUE_WIDTH + 2, OPAQUE_HEIGHT + 4, "OPAQUE", MAKE_COLOR(255, 255, 0));  

    drawWindow(drawAreaPosX - 1, drawAreaPosY - 3, drawAreaWidth + 2, drawAreaHeight + 4, "SPRITE", MAKE_COLOR(255, 255, 0)); 

    drawWindow(SLOTS_POSITION_X - 1, SLOTS_POSITION_Y - 1, SLOTS_WIDTH + 2, SLOTS_HEIGHT + 2, NULL, MAKE_COLOR(255, 255, 0));
    
    drawWindow(TOOLS_POSITION_X - 1, TOOLS_POSITION_Y - 3, TOOLS_WIDTH + 2, TOOLS_HEIGHT + 4, "TOOLS", MAKE_COLOR(255, 255, 0));    

    drawWindow(MODES_POSITION_X - 1, MODES_POSITION_Y - 3, MODES_WIDTH + 2, MODES_HEIGHT + 4, "MODES", MAKE_COLOR(255, 255, 0));    

    drawWindow(COMMANDS1_POSITION_X - 1, COMMANDS1_POSITION_Y - 3, COMMANDS1_WIDTH + 2, COMMANDS1_HEIGHT + 4, "COMMAND", MAKE_COLOR(255, 255, 0)); 
        
    drawWindow(COMMANDS2_POSITION_X - 1, COMMANDS2_POSITION_Y - 3, COMMANDS2_WIDTH + 2, COMMANDS2_HEIGHT + 4, "COMMAND", MAKE_COLOR(255, 255, 0)); 

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

    drawString(selectedOpacity ? COLOR_SELECTED : COLOR_UNSELECTED, "TRUE", OPAQUE_POSITION_X, OPAQUE_POSITION_Y);
    drawString(!selectedOpacity ? COLOR_SELECTED : COLOR_UNSELECTED, "FALSE", OPAQUE_POSITION_X + OPAQUE_WIDTH / 2, OPAQUE_POSITION_Y);


    for(int y = 0; y < drawAreaHeight; y++)
    {
        for(int x = 0; x < drawAreaWidth; x ++)
        {
            if((spriteCells[selectedSlot][y][x].color & (1 << 24)) != 0)
            {
                setScreenCell(drawAreaPosX + x, drawAreaPosY + y, spriteCells[selectedSlot][y][x].color, spriteCells[selectedSlot][y][x].character);
            }
        }
    }

    for(int x = 0; x < SLOTS_COUNT; x ++)
    {
        int color;
        if(x == selectedSlot) { color = COLOR_SELECTED; }
        else { color = COLOR_UNSELECTED; }             

        setScreenCell(SLOTS_POSITION_X + x * 2, SLOTS_POSITION_Y, color, x <= 9 ? '0' + x : 'A' + x - 10);
    }
    
    drawString(selectedTool == TOOL_SELECT ? COLOR_SELECTED : COLOR_UNSELECTED, " SELECT", TOOLS_POSITION_X, TOOLS_POSITION_Y + 0);
    drawString(selectedTool == TOOL_DRAW ? COLOR_SELECTED : COLOR_UNSELECTED, " DRAW", TOOLS_POSITION_X, TOOLS_POSITION_Y + 1);
    drawString(selectedTool == TOOL_PICK ? COLOR_SELECTED : COLOR_UNSELECTED, " PICK", TOOLS_POSITION_X, TOOLS_POSITION_Y + 2);
    drawString(selectedTool == TOOL_PIVOT ? COLOR_SELECTED : COLOR_UNSELECTED, " PIVOT", TOOLS_POSITION_X, TOOLS_POSITION_Y + 3);
    drawString(selectedTool == TOOL_BOUNDS ? COLOR_SELECTED : COLOR_UNSELECTED, " BOUNDS", TOOLS_POSITION_X, TOOLS_POSITION_Y + 4);

    drawString(selectedMode == MODE_ALL ? COLOR_SELECTED : COLOR_UNSELECTED, " ALL", MODES_POSITION_X, MODES_POSITION_Y + 0);
    drawString(selectedMode == MODE_CHAR ? COLOR_SELECTED : COLOR_UNSELECTED, " ASCII", MODES_POSITION_X, MODES_POSITION_Y + 1);
    drawString(selectedMode == MODE_COLOR ? COLOR_SELECTED : COLOR_UNSELECTED, " COLOR", MODES_POSITION_X, MODES_POSITION_Y + 2);
    drawString(selectedMode == MODE_OPACITY ? COLOR_SELECTED : COLOR_UNSELECTED, " OPACITY", MODES_POSITION_X, MODES_POSITION_Y + 3);

    int highlight = (commandHighlightedTimer > 0);
    
    int highlightNew = highlight && (commandHighlighted == COMMAND_NEW);
    int highlightLoad = highlight && (commandHighlighted == COMMAND_LOAD);
    int highlightSave = highlight && (commandHighlighted == COMMAND_SAVE);
    int highlightCopy = highlight && (commandHighlighted == COMMAND_COPY);
    int highlightCut = highlight && (commandHighlighted == COMMAND_CUT);
    int highlightPaste = highlight && (commandHighlighted == COMMAND_PASTE);
    int highlightClear = highlight && (commandHighlighted == COMMAND_CLEAR);
    int highlightMirrorX = highlight && (commandHighlighted == COMMAND_MIRROR_X);
    int highlightMirrorY = highlight && (commandHighlighted == COMMAND_MIRROR_Y);
	
    int highlightColor = commandHighlightedError ? COLOR_ERROR : COLOR_SELECTED; 
    
    drawString(COLOR_SELECTED, " F1", COMMANDS1_POSITION_X, COMMANDS1_POSITION_Y + 0);
    drawString(highlightNew ? highlightColor : COLOR_UNSELECTED, "NEW", COMMANDS1_POSITION_X + 6, COMMANDS1_POSITION_Y + 0);

    drawString(COLOR_SELECTED, " F2", COMMANDS1_POSITION_X, COMMANDS1_POSITION_Y + 1);
    drawString(highlightLoad ? highlightColor : COLOR_UNSELECTED, "LOAD", COMMANDS1_POSITION_X + 6, COMMANDS1_POSITION_Y + 1);

    drawString(COLOR_SELECTED, " F3", COMMANDS1_POSITION_X, COMMANDS1_POSITION_Y + 2);
    drawString(highlightSave ? highlightColor : COLOR_UNSELECTED, "SAVE", COMMANDS1_POSITION_X + 6, COMMANDS1_POSITION_Y + 2);

    drawString(COLOR_SELECTED, " ESC", COMMANDS1_POSITION_X, COMMANDS1_POSITION_Y + 3);
    drawString(COLOR_UNSELECTED, "EXIT", COMMANDS1_POSITION_X + 6, COMMANDS1_POSITION_Y + 3);
   
    drawString(COLOR_SELECTED, " CT+C", COMMANDS2_POSITION_X, COMMANDS2_POSITION_Y + 0);
    drawString(highlightCopy ? highlightColor : COLOR_UNSELECTED, "COPY", COMMANDS2_POSITION_X + 6, COMMANDS2_POSITION_Y + 0);
    drawString(COLOR_SELECTED, " CT+X", COMMANDS2_POSITION_X, COMMANDS2_POSITION_Y + 1);
    drawString(highlightCut ? highlightColor : COLOR_UNSELECTED, "CUT", COMMANDS2_POSITION_X + 6, COMMANDS2_POSITION_Y + 1);
    drawString(COLOR_SELECTED, " CT+V", COMMANDS2_POSITION_X, COMMANDS2_POSITION_Y + 2);
    drawString(highlightPaste ? highlightColor : COLOR_UNSELECTED, "PASTE", COMMANDS2_POSITION_X + 6, COMMANDS2_POSITION_Y + 2);
    drawString(COLOR_SELECTED, " C", COMMANDS2_POSITION_X, COMMANDS2_POSITION_Y + 3);
    drawString(highlightClear ? highlightColor : COLOR_UNSELECTED, "CLEAR", COMMANDS2_POSITION_X + 6, COMMANDS2_POSITION_Y + 3);
    drawString(COLOR_SELECTED, " X", COMMANDS2_POSITION_X, COMMANDS2_POSITION_Y + 4);
    drawString(highlightMirrorX ? highlightColor : COLOR_UNSELECTED, "MIRRX", COMMANDS2_POSITION_X + 6, COMMANDS2_POSITION_Y + 4);
    drawString(COLOR_SELECTED, " Y", COMMANDS2_POSITION_X, COMMANDS2_POSITION_Y + 5);
    drawString(highlightMirrorY ? highlightColor : COLOR_UNSELECTED, "MIRRY", COMMANDS2_POSITION_X + 6, COMMANDS2_POSITION_Y + 5);

}

void setMarqueeTopLeft(int x, int y, int* mX, int* mY, int* mW, int* mH)
{
	if(x <= *mX + *mW - 1)
	{
		*mX = x;
		*mW = *mW - (x - *mX);
		if(*mW <= 0) { printf("NEGATIVE Width"); exit(-1); }
	}
	else
	{
		*mX = x;
		*mW = 1;
	}
	
	if(y <= *mY + *mH - 1)
	{
		*mY  = y;
		*mH = *mH - (y - *mY);
		if(*mH < 0) { printf("NEGATIVE Height"); exit(-1); }
	}
	else
	{
		*mY = y;
		*mH = 1;
	}
	
	if(*mX + *mW - 1 > drawAreaWidth - 1)
	{
		*mW = drawAreaWidth - *mX;		
	}
	
	if(*mY + *mH - 1 > drawAreaHeight - 1)
	{
		*mH = drawAreaHeight - *mY;		
	}
}

void setMarqueeBottomRight(int x, int y, int* mX, int* mY, int* mW, int* mH)
{
	*mW = x < *mX ? 1 : x - *mX + 1;
	*mH = y < *mY ? 1 : y - *mY + 1;                        

}

void intersectMarquee(int &x1, int &y1, int &w1, int &h1, int x2, int y2, int w2, int h2)
{
	
}

void drawMarquee(int mX, int mY, int mW, int mH, int mC)
{
	int dX = drawAreaPosX;
	int dY = drawAreaPosY;
	
	setScreenCell(dX + mX,          dY + mY, mC, '+');
	setScreenCell(dX + mX + mW - 1, dY + mY, mC, '+');
	setScreenCell(dX + mX,          dY + mY + mH - 1, mC, '+');
	setScreenCell(dX + mX + mW - 1, dY + mY + mH - 1, mC, '+');
	
	for(int x = 1; x - 1 < mW - 2; x ++)
	{
		setScreenCell(dX + mX + x, dY + mY, mC, '-');
		setScreenCell(dX + mX + x, dY + mY + mH - 1, mC, '-');
	}
	
	for(int y = 1; y - 1 < mH - 2; y ++)
	{
		setScreenCell(dX + mX,          dY + mY + y, mC, '|');
		setScreenCell(dX + mX + mW - 1, dY + mY + y, mC, '|');
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
        
        fread(&spritePivots[slot * 2 + 0], sizeof(int), 1, file);
        fread(&spritePivots[slot * 2 + 1], sizeof(int), 1, file);
        
        fread(&spriteBounds[slot * 4 + 0], sizeof(int), 1, file);
        fread(&spriteBounds[slot * 4 + 1], sizeof(int), 1, file);
        fread(&spriteBounds[slot * 4 + 2], sizeof(int), 1, file);
        fread(&spriteBounds[slot * 4 + 3], sizeof(int), 1, file);        
        
        for(int y = 0; y < drawAreaHeight; y++)
        {
            for(int x = 0; x < drawAreaWidth; x++)
            {
                fread(&spriteCells[slot][y][x].character, sizeof(char), 1, file);
                fread(&spriteCells[slot][y][x].color, sizeof(int), 1, file);
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
        
        fwrite(&spritePivots[slot * 2 + 0], sizeof(int), 1, file);
        fwrite(&spritePivots[slot * 2 + 1], sizeof(int), 1, file);
        
        fwrite(&spriteBounds[slot * 4 + 0], sizeof(int), 1, file);
        fwrite(&spriteBounds[slot * 4 + 1], sizeof(int), 1, file);
        fwrite(&spriteBounds[slot * 4 + 2], sizeof(int), 1, file);
        fwrite(&spriteBounds[slot * 4 + 3], sizeof(int), 1, file);
        
        for(int y = 0; y < drawAreaHeight; y++)
        {
            for(int x = 0; x < drawAreaWidth; x++)
            {
                fwrite(&spriteCells[slot][y][x].character, sizeof(char), 1, file);
                fwrite(&spriteCells[slot][y][x].color, sizeof(int), 1, file);
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
    initUI();
    initMenu();
	
	
	
	char titleString[TITLE_STRING_SIZE];
	sprintf(titleString, TITLE_STRING_PATTERN, "Sprite Editor", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, "Jose M Solis");
    setScreenTitle(titleString);
    
    resizeScreen(CELLS_WIDTH * fontWidth, CELLS_HEIGHT * fontHeight);
    
    int quit = 0;
    
    cursorChar = 'O';
    cursorColor = MAKE_COLOR(255, 255, 255);
    
    selectedTool = TOOL_DRAW;
    selectedMode = MODE_ALL;
    
    selectedSlot = 0;
    selectedRedLevel = COLOR_PALETTE1_WIDTH - 1;
    selectedGreenLevel = COLOR_PALETTE1_HEIGHT - 1;
    selectedBlueLevel = COLOR_PALETTE2_WIDTH - 1;
    selectedAsciiLevel = '#' - ASCII_FIRST;
    selectedOpacity = 1;
	
	selectionPosX = 0;
	selectionPosY = 0;
	selectionWidth = drawAreaWidth;
	selectionHeight = drawAreaHeight;
    
    commandHighlighted = 0;
    commandHighlightedTimer = 0;
    commandHighlightedError = 0;

    for(int s = 0; s < SLOTS_COUNT; s++)
    {
        spriteCells[s] = (SpriteCell**)malloc(sizeof(SpriteCell*) * drawAreaHeight);
        for(int i = 0; i < drawAreaHeight; i ++)
        { 
            spriteCells[s][i] = (SpriteCell*)malloc(sizeof(SpriteCell) * drawAreaWidth);
            
            for(int j = 0; j < drawAreaWidth; j++)
            {
                spriteCells[s][i][j].color = 0;
                spriteCells[s][i][j].character = ' ';
            }
        }
        
        spritePivots[s * 2 + 0] = drawAreaWidth / 2;
        spritePivots[s * 2 + 1] = drawAreaHeight / 2;
        
        spriteBounds[s * 4 + 0] = 0;
        spriteBounds[s * 4 + 1] = 0;
        spriteBounds[s * 4 + 2] = drawAreaWidth;
        spriteBounds[s * 4 + 3] = drawAreaHeight;
    }
	
	copyBufferPosX = 0;
	copyBufferPosY = 0;

	copyBufferWidth = 0;
	copyBufferHeight = 0;

	copyBufferHasColor = 0;
	copyBufferHasCharacters = 0;
	copyBufferHasOpacity = 0;
    
	copyBuffer = (SpriteCell**)malloc(sizeof(SpriteCell*) * drawAreaHeight);
	for(int y = 0; y < drawAreaHeight; y ++)
	{ 
		copyBuffer[y] = (SpriteCell*)malloc(sizeof(SpriteCell) * drawAreaWidth);
		
		for(int x = 0; x < drawAreaWidth; x++)
		{
			copyBuffer[y][x].color = MAKE_COLOR_OPACITY(0,0,0,0);
			copyBuffer[y][x].character = ' ';
		}
	}
    
    for(int i = 0; i < SLOTS_COUNT; i++)
    {
        tryLoadSprite(i);
    }
        
    while(!quit)
    {
        int cursorWindowX;
        int cursorWindowY;
        getMousePosition(&cursorWindowX, &cursorWindowY);
        
        cursorCellX = cursorWindowX / fontWidth;
        cursorCellY = cursorWindowY / fontHeight;
		
		int colorModeEnabled = (selectedMode == MODE_COLOR || selectedMode == MODE_ALL);
		int characterModeEnabled = (selectedMode == MODE_CHAR || selectedMode == MODE_ALL);
		int opacityModeEnabled = (selectedMode == MODE_OPACITY || selectedMode == MODE_ALL);
		
		if(!isKeyPressed(KEY_ALT))
		{
			if(isKeyDown(KEY_0)) { selectedSlot = 0; }
			else if(isKeyDown(KEY_1)) { selectedSlot = 1; }
			else if(isKeyDown(KEY_2)) { selectedSlot = 2; }
			else if(isKeyDown(KEY_3)) { selectedSlot = 3; }
			else if(isKeyDown(KEY_4)) { selectedSlot = 4; }
			else if(isKeyDown(KEY_5)) { selectedSlot = 5; }
			else if(isKeyDown(KEY_6)) { selectedSlot = 6; }
			else if(isKeyDown(KEY_7)) { selectedSlot = 7; }
			else if(isKeyDown(KEY_8)) { selectedSlot = 8; }
			else if(isKeyDown(KEY_9)) { selectedSlot = 9; }
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
                    spriteCells[selectedSlot][y][x].character = ' ';
                    spriteCells[selectedSlot][y][x].color = 0;                    
                }
            }
            
            spritePivots[selectedSlot * 2 + 0] = drawAreaWidth / 2;
            spritePivots[selectedSlot * 2 + 1] = drawAreaHeight / 2;
            
            spriteBounds[selectedSlot * 4 + 0] = 0;
            spriteBounds[selectedSlot * 4 + 1] = 0;
            spriteBounds[selectedSlot * 4 + 2] = drawAreaWidth;
            spriteBounds[selectedSlot * 4 + 3] = drawAreaHeight;
            
            
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
		else if(isKeyPressed(KEY_CONTROL) && (isKeyDown(KEY_C) || isKeyDown(KEY_X)))
		{
			int isCut = isKeyPressed(KEY_X);
			
			copyBufferPosX = selectionPosX;
			copyBufferPosY = selectionPosY;
			copyBufferWidth = selectionWidth;
			copyBufferHeight = selectionHeight;
			
			copyBufferHasColor = colorModeEnabled;
			copyBufferHasOpacity = opacityModeEnabled;
			copyBufferHasCharacters = characterModeEnabled;

			for(int y = 0; y < copyBufferHeight; y ++)
			{
				for(int x = 0; x < copyBufferWidth; x++)
				{
					int col = spriteCells[selectedSlot][selectionPosY + y][selectionPosX + x].color;					
					int r = GET_COLOR_R(col);
					int g = GET_COLOR_G(col);
					int b = GET_COLOR_B(col);
					int o = GET_OPACITY(col);
					char c = spriteCells[selectedSlot][selectionPosY + y][selectionPosX + x].character;
					
					if(colorModeEnabled)
					{						
						copyBuffer[y + copyBufferPosY][x + copyBufferPosX].color &= 0xFF000000;
						copyBuffer[y + copyBufferPosY][x + copyBufferPosX].color |= MAKE_COLOR(r, g, b);
						
						if(isCut) { spriteCells[selectedSlot][y + copyBufferPosY][x + copyBufferPosX].color &= 0xFF000000; }
					}
					
					if(opacityModeEnabled)
					{
						copyBuffer[y + copyBufferPosY][x + copyBufferPosX].color &= 0x00FFFFFF;
						copyBuffer[y + copyBufferPosY][x + copyBufferPosX].color |= o << 24;

						if(isCut) { spriteCells[selectedSlot][y + selectionPosY][x + selectionPosX].color &= 0x00FFFFFF; }
					}
					
					if(characterModeEnabled)
					{
						copyBuffer[y + copyBufferPosY][x + copyBufferPosX].character = c;

						if(isCut) { spriteCells[selectedSlot][y + selectionPosY][x + selectionPosX].character = ' '; }
					}
				}
			}
			
            commandHighlighted = isCut ? COMMAND_CUT : COMMAND_COPY;
            commandHighlightedError = 0;
            commandHighlightedTimer = COMMAND_HIGHLIGHT_TIME;
			
				
		}
		else if(isKeyPressed(KEY_CONTROL) && isKeyDown(KEY_V))
		{
			int copyColors = colorModeEnabled && copyBufferHasColor;
			int copyOpacity = opacityModeEnabled && copyBufferHasOpacity;
			int copyChars = characterModeEnabled && copyBufferHasCharacters;
			
			int success = 0;
									
			if((copyBufferHasCharacters || copyBufferHasColor || copyBufferHasCharacters))				
			{
				for(int y = 0; y < copyBufferHeight; y ++)
				{
					for(int x = 0; x < copyBufferWidth; x++)
					{
						int col = copyBuffer[copyBufferPosY + y][copyBufferPosX + x].color;
						int r = GET_COLOR_R(col);
						int g = GET_COLOR_G(col);
						int b = GET_COLOR_B(col);
						int o = GET_OPACITY(col);
						char c = copyBuffer[copyBufferPosY + y][copyBufferPosX + x].character;
						
						int spriteX = cursorCellX - drawAreaPosX + x;
						int spriteY = cursorCellY - drawAreaPosY + y;
						
						if(isInsideRect(spriteX, spriteY, 0, 0, drawAreaWidth, drawAreaHeight))
						{
							if(copyColors)
							{
								spriteCells[selectedSlot][spriteY][spriteX].color &= 0xFF000000;
								spriteCells[selectedSlot][spriteY][spriteX].color |= MAKE_COLOR(r, g, b);
							}
							
							if(copyOpacity)
							{
								spriteCells[selectedSlot][spriteY][spriteX].color &= 0x00FFFFFF;
								spriteCells[selectedSlot][spriteY][spriteX].color |= o << 24;
							}
							
							if(copyChars)
							{
								spriteCells[selectedSlot][spriteY][spriteX].character = c;
							}
							
							success = 1;
						}
					}
				}
				
			}
			
			if(success)
			{
				selectionPosX = (cursorCellX - drawAreaPosX);
				selectionPosY = (cursorCellY - drawAreaPosY);
				selectionWidth = copyBufferWidth;
				selectionHeight = copyBufferHeight;
				
				if(selectionPosX < 0)
				{
					selectionWidth += selectionPosX;
					selectionWidth = max(selectionWidth, 1);
					selectionPosX = 0;
				}
				else if(selectionPosX > drawAreaWidth - 1)
				{
					selectionPosX = drawAreaWidth - 1;
				}
				
				if(selectionPosX + selectionWidth - 1 > drawAreaWidth - 1)
				{
					selectionWidth = drawAreaWidth - selectionPosX;
				}

				if(selectionPosY < 0)
				{
					selectionHeight += selectionPosY;
					selectionHeight = max(selectionHeight, 1);
					selectionPosY = 0;
				}
				else if(selectionPosY > drawAreaHeight - 1)
				{
					selectionPosY = drawAreaHeight - 1;
				}
				
				if(selectionPosY + selectionHeight - 1 > drawAreaHeight - 1)
				{ 
					selectionHeight = drawAreaHeight - selectionPosY;
				}					
			}

			
            commandHighlighted = COMMAND_PASTE;
            commandHighlightedError = !success;
            commandHighlightedTimer = COMMAND_HIGHLIGHT_TIME;
				
		}
		else if(isKeyDown(KEY_X) || isKeyDown(KEY_Y))
		{
			int isMirrorX = isKeyPressed(KEY_X);
			int mirrorWidth = isMirrorX ? selectionWidth / 2 : selectionWidth;
			int mirrorHeight = isMirrorX ? selectionHeight : selectionHeight / 2;
	
			for(int y = 0; y < mirrorHeight; y ++)
			{
				for(int x = 0; x < mirrorWidth; x++)
				{
					int sourceX = isMirrorX ? selectionPosX + selectionWidth - 1 - x : selectionPosX + x;
					int sourceY = isMirrorX ? selectionPosY + y : selectionPosY + selectionHeight - 1 - y;
					
					int scol = spriteCells[selectedSlot][sourceY][sourceX].color;
					int sr = GET_COLOR_R(scol);
					int sg = GET_COLOR_G(scol);
					int sb = GET_COLOR_B(scol);
					int so = GET_OPACITY(scol);
					char sc = spriteCells[selectedSlot][sourceY][sourceX].character;
					
					int targetX = selectionPosX + x;
					int targetY = selectionPosY + y;

					int tcol = spriteCells[selectedSlot][targetY][targetX].color;
					int tr = GET_COLOR_R(tcol);
					int tg = GET_COLOR_G(tcol);
					int tb = GET_COLOR_B(tcol);
					int to = GET_OPACITY(tcol);
					char tc = spriteCells[selectedSlot][targetY][targetX].character;

					if(colorModeEnabled)
					{
						spriteCells[selectedSlot][sourceY][sourceX].color &= 0xFF000000;
						spriteCells[selectedSlot][sourceY][sourceX].color |= MAKE_COLOR(tr, tg, tb);

						spriteCells[selectedSlot][targetY][targetX].color &= 0xFF000000;
						spriteCells[selectedSlot][targetY][targetX].color |= MAKE_COLOR(sr, sg, sb);
					}
					
					if(opacityModeEnabled)
					{
						spriteCells[selectedSlot][sourceY][sourceX].color &= 0x00FFFFFF;
						spriteCells[selectedSlot][sourceY][sourceX].color |= to << 24;

						spriteCells[selectedSlot][targetY][targetX].color &= 0x00FFFFFF;
						spriteCells[selectedSlot][targetY][targetX].color |= so << 24;
					}
					
					if(characterModeEnabled)
					{
						spriteCells[selectedSlot][sourceY][sourceX].character = tc;
						
						spriteCells[selectedSlot][targetY][targetX].character = sc;
					}
				}
			}					

            commandHighlighted = isMirrorX ? COMMAND_MIRROR_X : COMMAND_MIRROR_Y;
            commandHighlightedError = 0;
            commandHighlightedTimer = COMMAND_HIGHLIGHT_TIME;
		}
		else if(!isKeyPressed(KEY_CONTROL) && isKeyDown(KEY_C))
		{
			for(int y = 0; y < selectionHeight; y ++)
			{
				for(int x = 0; x < selectionWidth; x++)
				{
					int cellX = selectionPosX + x;
					int cellY = selectionPosY + y;
					
					int col = spriteCells[selectedSlot][cellY][cellX].color;					
					int r = GET_COLOR_R(col);
					int g = GET_COLOR_G(col);
					int b = GET_COLOR_B(col);
					int o = GET_OPACITY(col);
					char c = spriteCells[selectedSlot][cellY][cellX].character;
					
					if(colorModeEnabled)
					{						
						spriteCells[selectedSlot][cellY][cellX].color &= 0xFF000000;
						spriteCells[selectedSlot][cellY][cellX].color |= MAKE_COLOR(0, 0, 0);
					}
					
					if(opacityModeEnabled)
					{
						spriteCells[selectedSlot][cellY][cellX].color &= 0x00FFFFFF;
						spriteCells[selectedSlot][cellY][cellX].color |= 0 << 24;
					}
					
					if(characterModeEnabled)
					{
						spriteCells[selectedSlot][cellY][cellX].character = ' ';
					}
				}
			}				

            commandHighlighted = COMMAND_CLEAR;
            commandHighlightedError = 0;
            commandHighlightedTimer = COMMAND_HIGHLIGHT_TIME;
		}
        
        int mouseLeftPressed = isKeyPressed(MOUSE_LEFT) ? 1 : 0;
        int mouseRightPressed = isKeyPressed(MOUSE_RIGHT) ? 1 : 0;
                
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
                int canChangeOpacity = (selectedMode == MODE_ALL || selectedMode == MODE_OPACITY);
                int isOpacityMode = (selectedMode == MODE_OPACITY);

				if(selectedTool == TOOL_SELECT)
				{                        
                    if(mouseLeftPressed)
                    {
						setMarqueeTopLeft(x, y, &selectionPosX, &selectionPosY, &selectionWidth, &selectionHeight);                        
                    }
                    else
                    {
						setMarqueeBottomRight(x, y, &selectionPosX, &selectionPosY, &selectionWidth, &selectionHeight);                        
                    }
										
				}
                if(selectedTool == TOOL_DRAW)
                {
                    if(mouseLeftPressed)
                    {  
                        int r = (int)(selectedRedLevel * (255.0f / COLOR_PALETTE1_WIDTH));
                        int g = (int)(selectedGreenLevel * (255.0f / COLOR_PALETTE1_HEIGHT));
                        int b = (int)(selectedBlueLevel * (255.0f / COLOR_PALETTE2_WIDTH));
                        
                        char c = ASCII_FIRST + selectedAsciiLevel;

						if(isInsideRect(x, y, selectionPosX, selectionPosY, selectionWidth, selectionHeight))
						{
							if(canChangeColor)
							{
								int color = spriteCells[s][y][x].color;
								
								int opacity = color >> 24;
								
								spriteCells[s][y][x].color = MAKE_COLOR(r, g, b) | (opacity << 24);
							}
							if(canChangeCharacter) { spriteCells[s][y][x].character = c; }
							if(canChangeOpacity)
							{
								int color = spriteCells[s][y][x].color;
								
								if(selectedOpacity) { color |= (1 << 24); }
								else { color &= ~(1 << 24); }
								
								spriteCells[s][y][x].color = color;
							}
						}
                        
                    }
                    else
                    {
						if(isInsideRect(x, y, selectionPosX, selectionPosY, selectionWidth, selectionHeight))
						{
							if(canChangeColor) { spriteCells[s][y][x].color = MAKE_COLOR(0, 0, 0); }
							if(canChangeCharacter) { spriteCells[s][y][x].character = ' '; }                        
							if(canChangeOpacity) { spriteCells[s][y][x].color &= ~(1 << 24); }
						}                        
                    }
                }
                else if(selectedTool == TOOL_PICK)
                {
                    if(mouseLeftPressed)
                    {                      
                        int color = spriteCells[s][y][x].color;
                        int c = spriteCells[s][y][x].character;
                        
                        if(canChangeCharacter) { selectedAsciiLevel = c - ASCII_FIRST; }
                        if(canChangeColor)
                        {   selectedRedLevel = (int)(GET_COLOR_R(color) / (255.0f / COLOR_PALETTE1_WIDTH));
                            selectedGreenLevel = (int)(GET_COLOR_G(color) / (255.0f / COLOR_PALETTE1_HEIGHT));
                            selectedBlueLevel = (int)(GET_COLOR_B(color) / (255.0f / COLOR_PALETTE2_WIDTH));
                        }
                        if(canChangeOpacity) { selectedOpacity = (color & (1 << 24)) != 0 ? 1 : 0; }
                    }
                }
                else if(selectedTool == TOOL_PIVOT)
                {
                    if(mouseLeftPressed)
                    {                      
                        spritePivots[selectedSlot * 2 + 0] = x;
                        spritePivots[selectedSlot * 2 + 1] = y;
                    }
                }
                else if(selectedTool == TOOL_BOUNDS)
                {
                    int bX = spriteBounds[selectedSlot * 4 + 0];
                    int bY = spriteBounds[selectedSlot * 4 + 1]; 
                    int bW = spriteBounds[selectedSlot * 4 + 2];
                    int bH = spriteBounds[selectedSlot * 4 + 3];
                        
                    if(mouseLeftPressed)
                    {
						setMarqueeTopLeft(x, y, &bX, &bY, &bW, &bH);                        
                    }
                    else
                    {
						setMarqueeBottomRight(x, y, &bX, &bY, &bW, &bH);
                    }
					
                    spriteBounds[selectedSlot * 4 + 0] = bX;
                    spriteBounds[selectedSlot * 4 + 1] = bY; 
                    spriteBounds[selectedSlot * 4 + 2] = bW;
                    spriteBounds[selectedSlot * 4 + 3] = bH;
					
                    
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
        else if(isInsideRect(cursorCellX, cursorCellY,
                             OPAQUE_POSITION_X, OPAQUE_POSITION_Y,
                             OPAQUE_WIDTH, OPAQUE_HEIGHT))
        {
            if(mouseLeftPressed)
            {            
                selectedOpacity = 1 - (cursorCellX - OPAQUE_POSITION_X) / (OPAQUE_WIDTH / 2);
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
        
        if(selectedMode == MODE_OPACITY)
        {
            for(int y = 0; y < drawAreaHeight; y++)
            {
                for(int x = 0; x < drawAreaWidth; x++)
                {
                    if((spriteCells[selectedSlot][y][x].color & (1 << 24)) == 0)
                    {
                        setScreenCell(drawAreaPosX + x, drawAreaPosY + y, MAKE_COLOR(255, 0, 255), '#');
                    }
                }
            }
        }
		
		int isFullSelection = (selectionPosX == 0 && selectionPosY == 0 && selectionWidth == drawAreaWidth && selectionHeight == drawAreaHeight);
        
		if(selectedTool == TOOL_SELECT || (!isFullSelection && selectedTool == TOOL_DRAW))
		{
			drawMarquee(selectionPosX, selectionPosY, selectionWidth, selectionHeight, COLOR_SELECTED);			
		}
        else if(selectedTool == TOOL_BOUNDS)
        {
            int bX = spriteBounds[selectedSlot * 4 + 0];
            int bY = spriteBounds[selectedSlot * 4 + 1];
            int bW = spriteBounds[selectedSlot * 4 + 2];
            int bH = spriteBounds[selectedSlot * 4 + 3];
			
			drawMarquee(bX, bY, bW, bH, COLOR_BOUNDS);
			
        }

        if(selectedTool == TOOL_PIVOT || selectedTool == TOOL_BOUNDS)
        {
            setScreenCell(drawAreaPosX + spritePivots[selectedSlot * 2 + 0],
                          drawAreaPosY + spritePivots[selectedSlot * 2 + 1], COLOR_BOUNDS, '+');
        }        
        
        setScreenCell(cursorCellX, cursorCellY, cursorColor, cursorChar);
        
        showScreen();
        
        wait((int)(1.0f / SCREEN_FPS * 1000));
    }
    
}