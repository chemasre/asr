#include "screen.hpp"
#include <input.hpp>


HANDLE consoleHandle;
HWND windowHandle;

int screenWidth;
int screenHeight;

int screenWindowWidth;
int screenWindowHeight;

int screenMonitorWidth;
int screenMonitorHeight;

int screenMaxWindowWidth;
int screenMaxWindowHeight;

int fontWidth;
int fontHeight;

int clearColor;

ScreenCell* screenLine;
ScreenCell** screen;

char showScreenCellBuffer[10];
char showScreenBuffer[1024 * 1024];

int frameCounter;


void resizeScreen(int windowWidth, int windowHeight)
{
    BOOL result;    
    
    LONG_PTR style = GetWindowLongPtr(windowHandle, GWL_STYLE);
    style &= ~(WS_MAXIMIZEBOX | WS_MINIMIZEBOX);
    result = SetWindowLongPtr(windowHandle, GWL_STYLE, style);    
    ASSERT(result, "Failed to set window style")
    
    DWORD mode;    
    result = GetConsoleMode(consoleHandle, &mode);
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    result = SetConsoleMode(consoleHandle, mode);
    ASSERT(result, "Failed to set console mode")
    
    int previousWidth = screenWidth;
    int previousHeight = screenHeight;    
    
    CONSOLE_FONT_INFO font;
    result = GetCurrentConsoleFont(consoleHandle, FALSE, &font);
    ASSERT(result, "Unable to get console font")
    
    fontWidth = font.dwFontSize.X;
    fontHeight = font.dwFontSize.Y; 

    printf("font width %d height%d\n", fontWidth, fontHeight);
    
   
    MoveWindow(windowHandle, screenMonitorWidth / 2 - windowWidth / 2, screenMonitorHeight / 2 - windowHeight / 2, windowWidth, windowHeight,  TRUE);
    ShowScrollBar(windowHandle, SB_BOTH, FALSE);
    
    COORD coord = GetLargestConsoleWindowSize(consoleHandle);
    screenMaxWindowWidth = coord.X * fontWidth - 1;
    screenMaxWindowHeight = coord.Y * fontHeight - 1;
    
    screenWidth = windowWidth / fontWidth;
    screenHeight = windowHeight / fontHeight;
    
    if(screenWidth >= coord.X - 1) { screenWidth = coord.X - 1; }
    if(screenHeight>= coord.Y - 1) { screenHeight = coord.Y - 1; }
    
    printf("screen width %d height %d\n", screenWidth, screenHeight);

    printf("max screen width %d height %d\n", coord.X, coord.Y);
    
    if(screenLine != 0) { free(screenLine); }
    
    if(screen != 0)
    { 
        for(int i = 0; i < previousHeight; i++)
        {
            free(screen[i]);
        }
        
        free(screen);
    }

    screenLine = (ScreenCell*)malloc(sizeof(ScreenCell) * screenWidth);
    screen = (ScreenCell**)malloc(sizeof(ScreenCell*) * screenHeight);
    for(int i = 0; i < screenHeight; i++)
    {
        screen[i] = (ScreenCell*)malloc(sizeof(ScreenCell) * screenWidth);
        
        for(int j = 0; j < screenWidth; j ++)
        {
            sprintf(screen[i][j].string, "\033[%dm%c\033[0m", 0, ' ');
        }
        
    }
    
    
    COORD consoleBufferSize = {(SHORT)(screenWidth + 1), (SHORT)(screenHeight + 1)};
    result = SetConsoleScreenBufferSize(consoleHandle, consoleBufferSize);
    ASSERT(result, "Unable to set console buffer size")    

    SMALL_RECT consoleSize = {0, 0, (SHORT)(screenWidth), (SHORT)(screenHeight) };
    result = SetConsoleWindowInfo(consoleHandle, TRUE, &consoleSize);
    
    printf("w %d h %d\n", screenWidth, screenHeight);
    ASSERT(result, "Unable to set console size")
    

    screenWindowWidth = windowWidth;
    screenWindowHeight = windowHeight;
    
}

void drawString(int color, char s[], int x, int y)
{
    int i;
    i = 0;
    
    while(s[i] != '\0')
    {
        if(x + i >= 0 && x + i < screenWidth)
        {
            sprintf(screen[y][x + i].string, "\033[38;2;%d;%d;%dm%c\033[0m", GET_COLOR_R(color), GET_COLOR_G(color), GET_COLOR_B(color), s[i]);
            
            screen[y][x + i].character = s[i];
            screen[y][x + i].color = color;
        }
        
        i ++;
    }

}

void setScreenCursorPosition(int x, int y)
{
    COORD coord;    
    coord.X = 0;
    coord.Y = y;
    SetConsoleCursorPosition(consoleHandle, coord);    
    
}

void setScreenCell(int cellX, int cellY, int color, char c)
{
    if(cellX >= 0 && cellX < screenWidth && cellY >= 0 && cellY < screenHeight)
    {
        sprintf(screen[cellY][cellX].string, "\033[38;2;%d;%d;%dm%c\033[0m", GET_COLOR_R(color), GET_COLOR_G(color), GET_COLOR_B(color), c);
        
        screen[cellY][cellX].character = c;
        screen[cellY][cellX].color = color;
        
    }    
}

ScreenCell getScreenCell(int cellX, int cellY, int boundaryColor, char boundaryCharacter)
{
    ScreenCell cell = ScreenCell();
    
    if(cellX >= 0 && cellX < screenWidth && cellY >= 0 && cellY < screenHeight)
    {
        cell.character = screen[cellY][cellX].character;
        cell.color = screen[cellY][cellX].character;
        strcpy(cell.string, screen[cellY][cellX].string);
    }
    else
    {
        sprintf(cell.string, "\033[38;2;%d;%d;%dm%c\033[0m", GET_COLOR_R(boundaryColor), GET_COLOR_G(boundaryColor), GET_COLOR_B(boundaryColor), boundaryCharacter);
        cell.character = boundaryCharacter;
        cell.color = boundaryColor;
    }
    
    return cell;
}

void fillScreenArea(int areaX, int blankAreaY, int areaWidth, int blankAreaHeight, int color, char c)
{
    for(int y = blankAreaY; y < blankAreaY + blankAreaHeight; y ++)
    {
        for(int x = areaX; x < areaX + areaWidth; x++)
        {
            if(x >= 0 && x < screenWidth && y >= 0 && y < screenHeight)
            {                
                sprintf(screen[y][x].string,  "\033[38;2;%d;%d;%dm%c\033[0m", GET_COLOR_R(color), GET_COLOR_G(color), GET_COLOR_B(color), c);
                
                screen[y][x].character = c;
                screen[y][x].color = color;
                
            }
        }           
       
    }    
}

void setClearColor(int color)
{
    clearColor = color;
    // printf("R%d G%d B%d\n", GET_COLOR_R(clearColor), GET_COLOR_G(clearColor), GET_COLOR_B(clearColor));
    
    // exit(0);
}

void clearScreen()
{
    for(int y = 0; y < screenHeight; y ++)
    {
        for(int x = 0; x < screenWidth; x++)
        {
            sprintf(screen[y][x].string,  "\033[48;2;%d;%d;%dm%c\033[0m", GET_COLOR_R(clearColor), GET_COLOR_G(clearColor), GET_COLOR_B(clearColor), ' ');
            
            screen[y][x].character = ' ';
            screen[y][x].color = clearColor;            
        }
    }
    
    showScreen();

}


void showScreen()
{
    showScreenBuffer[0] = '\0';    
    
    int i = 0;
    
    for(int y = 0; y < screenHeight; y ++)
    {
        for(int x = 0; x < screenWidth; x++)
        {
            char* c = screen[y][x].string;
            int j = 0;
            while(c[j] != '\0') { showScreenBuffer[i + j] = c[j]; j++; }
            i += j;
            
        }
        
        showScreenBuffer[i] = '\n';
        i++;
    }    
    
    showScreenBuffer[i] = '\0';
    
    setScreenCursorPosition(0, 0);
    printf(showScreenBuffer);
    
}


void initScreen()
{
    consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    windowHandle = GetConsoleWindow();
    
    screenLine = 0;
    screen = 0;
    
    screenMonitorWidth = GetSystemMetrics(SM_CXFULLSCREEN);
    screenMonitorHeight = GetSystemMetrics(SM_CYFULLSCREEN);    
    
    resizeScreen(SCREEN_MIN_WINDOW_WIDTH, SCREEN_MIN_WINDOW_HEIGHT);
    
    frameCounter = 0;
    
}

void setScreenTitle(char title[])
{
    SetConsoleTitle(title);
}

void updateScreen()
{
    int increaseResolution = isKeyPressed(KEY_PLUS);
    int decreaseResolution = isKeyPressed(KEY_MINUS);
    
    if(increaseResolution || decreaseResolution)
    {
        float heightStep = (screenMaxWindowHeight - SCREEN_MIN_WINDOW_HEIGHT) / (float)SCREEN_RESOLUTION_STEPS;
        float widthStep = (screenMaxWindowWidth - SCREEN_MIN_WINDOW_WIDTH) / (float)SCREEN_RESOLUTION_STEPS;
        
        int nextWindowHeight = screenWindowHeight;
        int nextWindowWidth = screenWindowWidth;
        
        if(increaseResolution)
        {
            nextWindowHeight = screenWindowHeight + heightStep;
            nextWindowWidth = screenWindowWidth + widthStep;
        }
        else
        {
            nextWindowHeight = screenWindowHeight - heightStep;
            nextWindowWidth = screenWindowWidth - widthStep;
        }
        
       
        if(nextWindowHeight < SCREEN_MIN_WINDOW_HEIGHT) { nextWindowHeight = SCREEN_MIN_WINDOW_HEIGHT; }
        else if(nextWindowHeight > screenMonitorHeight)  { nextWindowHeight = screenMonitorHeight; }

        if(nextWindowWidth < SCREEN_MIN_WINDOW_WIDTH) { nextWindowWidth = SCREEN_MIN_WINDOW_WIDTH; }
        else if(nextWindowWidth > screenMonitorWidth)  { nextWindowWidth = screenMonitorWidth; }
        
        clearScreen();
        resizeScreen(nextWindowWidth, nextWindowHeight);
    } 

    frameCounter ++;
}




