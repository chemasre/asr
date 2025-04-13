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

char* screenLine;

char** screen;

int frameCounter;


void resizeScreen(int windowWidth, int windowHeight)
{
    BOOL result;    
    
    LONG_PTR style = GetWindowLongPtr(windowHandle, GWL_STYLE);
    style &= ~(WS_MAXIMIZEBOX | WS_MINIMIZEBOX);
    result = SetWindowLongPtr(windowHandle, GWL_STYLE, style);    
    ASSERT(result, "Failed to set window style")
    
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

    screenLine = (char*)malloc(sizeof(char) * (screenWidth + 1));
    screen = (char**)malloc(sizeof(char*) * screenHeight);
    for(int i = 0; i < screenHeight; i++)
    {
        screen[i] = (char*)malloc(sizeof(char) * (screenWidth + 1));
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

void drawString(char s[], int x, int y)
{
    int i;
    i = 0;
    
    while(s[i] != '\0')
    {
        if(x + i >= 0 && x + i < screenWidth)
        {
            screen[y][x + i] = s[i];
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

void setScreenCell(int cellX, int cellY, char c)
{
    if(cellX >= 0 && cellX < screenWidth && cellY >= 0 && cellY < screenHeight)
    {
        screen[cellY][cellX] = c;
    }    
}

void fillScreenArea(int areaX, int blankAreaY, int areaWidth, int blankAreaHeight, char c)
{
    for(int y = blankAreaY; y < blankAreaY + blankAreaHeight; y ++)
    {
        for(int x = areaX; x < areaX + areaWidth; x++)
        {
            if(x >= 0 && x < screenWidth && y >= 0 && y < screenHeight)
            {
                screen[y][x] = c;
            }
        }           
       
    }    
}

void clearScreen()
{
    for(int y = 0; y < screenHeight; y ++)
    {
        for(int x = 0; x < screenWidth; x++) { screen[y][x] = ' '; }
        screen[y][screenWidth] = '\0';
           
        setScreenCursorPosition(0, y);
        printf(screen[y]);
        
    }

}


void showScreen()
{
    
    for(int y = 0; y < screenHeight; y ++)
    {
        setScreenCursorPosition(0, y);
        printf(screen[y]);
    }    
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




