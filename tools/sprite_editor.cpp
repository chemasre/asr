#include "sprite_editor.hpp"
#include "editor_utils.hpp"

#define CELLS_WIDTH 70
#define CELLS_HEIGHT 40

#define COLOR_OPAQUE MAKE_COLOR(255,0,0)

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

#define TOOL_DRAW 0
#define TOOL_PICK 1
#define TOOL_PIVOT 2
#define TOOL_BOUNDS 3

#define TOOLS_COUNT 4
#define TOOLS_WIDTH 10
#define TOOLS_HEIGHT TOOLS_COUNT
#define TOOLS_POSITION_X 3
#define TOOLS_POSITION_Y 4

#define MODE_ALL   0
#define MODE_CHAR  1
#define MODE_COLOR 2
#define MODE_OPACITY 3

#define MODES_COUNT 4
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
int selectedOpacity;

int drawAreaPosX = TOOLS_POSITION_X + TOOLS_WIDTH + 1 + 5;
int drawAreaPosY = 4;
int drawAreaWidth = 32;
int drawAreaHeight = 32;

char workPath[MAX_PATH_LENGTH];

SpriteCell** spriteCells[SLOTS_COUNT];
int spritePivots[SLOTS_COUNT * 2];
int spriteBounds[SLOTS_COUNT * 4];


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
    
    drawString(selectedTool == TOOL_DRAW ? COLOR_SELECTED : COLOR_UNSELECTED, " DRAW", TOOLS_POSITION_X, TOOLS_POSITION_Y + 0);
    drawString(selectedTool == TOOL_PICK ? COLOR_SELECTED : COLOR_UNSELECTED, " PICK", TOOLS_POSITION_X, TOOLS_POSITION_Y + 1);
    drawString(selectedTool == TOOL_PIVOT ? COLOR_SELECTED : COLOR_UNSELECTED, " PIVOT", TOOLS_POSITION_X, TOOLS_POSITION_Y + 2);
    drawString(selectedTool == TOOL_BOUNDS ? COLOR_SELECTED : COLOR_UNSELECTED, " BOUNDS", TOOLS_POSITION_X, TOOLS_POSITION_Y + 3);

    drawString(selectedMode == MODE_ALL ? COLOR_SELECTED : COLOR_UNSELECTED, " ALL", MODES_POSITION_X, MODES_POSITION_Y + 0);
    drawString(selectedMode == MODE_CHAR ? COLOR_SELECTED : COLOR_UNSELECTED, " ASCII", MODES_POSITION_X, MODES_POSITION_Y + 1);
    drawString(selectedMode == MODE_COLOR ? COLOR_SELECTED : COLOR_UNSELECTED, " COLOR", MODES_POSITION_X, MODES_POSITION_Y + 2);
    drawString(selectedMode == MODE_OPACITY ? COLOR_SELECTED : COLOR_UNSELECTED, " OPACITY", MODES_POSITION_X, MODES_POSITION_Y + 3);

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
    initMenu();
    
    setScreenTitle("Sprite editor");
    
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
    
    for(int i = 0; i < SLOTS_COUNT; i++)
    {
        tryLoadSprite(i);
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
                int canChangeOpacity = (selectedMode == MODE_ALL || selectedMode == MODE_OPACITY);
                int isOpacityMode = (selectedMode == MODE_OPACITY);


                if(selectedTool == TOOL_DRAW)
                {
                    if(mouseLeftPressed)
                    {  
                        int r = (int)(selectedRedLevel * (255.0f / COLOR_PALETTE1_WIDTH));
                        int g = (int)(selectedGreenLevel * (255.0f / COLOR_PALETTE1_HEIGHT));
                        int b = (int)(selectedBlueLevel * (255.0f / COLOR_PALETTE2_WIDTH));
                        
                        char c = ASCII_FIRST + selectedAsciiLevel;                
                        
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
                    else
                    {
                        if(canChangeColor) { spriteCells[s][y][x].color = MAKE_COLOR(0, 0, 0); }
                        if(canChangeCharacter) { spriteCells[s][y][x].character = ' '; }                        
                        if(canChangeOpacity) { spriteCells[s][y][x].color &= ~(1 << 24); }                        
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
                        if(x <= bX + bW - 1)
                        {
                            spriteBounds[selectedSlot * 4 + 0] = x;
                            spriteBounds[selectedSlot * 4 + 2] = bW - (x - bX);
                            if(spriteBounds[selectedSlot * 4 + 2] <= 0) { printf("NEGATIVE Width"); exit(-1); }
                        }
                        else
                        {
                            spriteBounds[selectedSlot * 4 + 0]  = bX;                            
                            spriteBounds[selectedSlot * 4 + 2] = 1;
                        }
                        
                        if(y <= bY + bH - 1)
                        {
                            spriteBounds[selectedSlot * 4 + 1]  = y;
                            spriteBounds[selectedSlot * 4 + 3] = bH - (y - bY);
                            if(spriteBounds[selectedSlot * 4 + 3] <= 0) { printf("NEGATIVE Height"); exit(-1); }
                        }
                        else
                        {
                            spriteBounds[selectedSlot * 4 + 1]  = bY;                            
                            spriteBounds[selectedSlot * 4 + 3] = 1;
                        }
                        
                        // if(x + bW >= drawAreaWidth) { bW = drawAreaWidth - x; }
                        // if(y + bH >= drawAreaHeight) { bH = drawAreaHeight - y; }
                        
                    }
                    else
                    {
                        spriteBounds[selectedSlot * 4 + 2] = x < bX ? 1 : x - bX + 1;
                        spriteBounds[selectedSlot * 4 + 3] = y < bY ? 1 : y - bY + 1;                        
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
        
        if(selectedTool == TOOL_BOUNDS)
        {
            int dX = drawAreaPosX;
            int dY = drawAreaPosY;
            int bX = spriteBounds[selectedSlot * 4 + 0];
            int bY = spriteBounds[selectedSlot * 4 + 1];
            int bW = spriteBounds[selectedSlot * 4 + 2];
            int bH = spriteBounds[selectedSlot * 4 + 3];
            
            setScreenCell(dX + bX,          dY + bY, COLOR_SELECTED, '+');
            setScreenCell(dX + bX + bW - 1, dY + bY, COLOR_SELECTED, '+');
            setScreenCell(dX + bX,          dY + bY + bH - 1, COLOR_SELECTED, '+');
            setScreenCell(dX + bX + bW - 1, dY + bY + bH - 1, COLOR_SELECTED, '+');
            
            for(int x = 1; x - 1 < bW - 2; x ++)
            {
                setScreenCell(dX + bX + x, dY + bY, COLOR_SELECTED, '-');
                setScreenCell(dX + bX + x, dY + bY + bH - 1, COLOR_SELECTED, '-');
            }
            
            for(int y = 1; y - 1 < bH - 2; y ++)
            {
                setScreenCell(dX + bX,          dY + bY + y, COLOR_SELECTED, '|');
                setScreenCell(dX + bX + bW - 1, dY + bY + y, COLOR_SELECTED, '|');
            }
        }

        if(selectedTool == TOOL_PIVOT || selectedTool == TOOL_BOUNDS)
        {
            setScreenCell(drawAreaPosX + spritePivots[selectedSlot * 2 + 0],
                          drawAreaPosY + spritePivots[selectedSlot * 2 + 1], COLOR_SELECTED, '+');
        }
        
        
        setScreenCell(cursorCellX, cursorCellY, cursorColor, cursorChar);
        
        showScreen();
        
        wait((int)(1.0f / SCREEN_FPS * 1000));
    }
    
}