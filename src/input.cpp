#include <input.hpp>

HWND input_windowHandle;
HANDLE input_consoleHandle;
HANDLE input_consoleHandleInput;

int inputMode;

int lastMousePositionX = 0;
int lastMousePositionY = 0;

int mouseDeltaX = 0;
int mouseDeltaY = 0;


int isKeyPressed(int k)
{
    return GetKeyState(k) & 0x8000;
}

int isKeyDown(int k)
{
    int s = GetAsyncKeyState(k);
    return  (s & 0x8000) && (s & 0x0001);
}

void updateInput()
{
    int mX;
    int mY;
    
    getMousePosition(&mX, &mY);
    
    mouseDeltaX = mX - lastMousePositionX;
    mouseDeltaY = mY - lastMousePositionY;

    lastMousePositionX = mX;
    lastMousePositionY = mY;
}


void initInput()
{
    BOOL result;
    
    input_windowHandle = GetConsoleWindow();
    input_consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    input_consoleHandleInput = GetStdHandle(STD_INPUT_HANDLE);
   
    DWORD consoleMode;
    result = GetConsoleMode(input_consoleHandleInput, &consoleMode); 
    ASSERT(result, "Failed to get console mode")   
    
    consoleMode = ENABLE_EXTENDED_FLAGS | (consoleMode & ~ENABLE_QUICK_EDIT_MODE);
    result = SetConsoleMode(input_consoleHandleInput, consoleMode);  
    ASSERT(result, "Failed to set console mode")
    
    getMousePosition(&lastMousePositionX, &lastMousePositionY);
    
}

void getMousePosition(int* x, int* y)
{
    BOOL result;    
    POINT position;
    
    result = GetCursorPos(&position);
    ASSERT(result, "Failed to get mouse position")    
    
    result = ScreenToClient(input_windowHandle, &position);
    ASSERT(result, "Failed to get mouse position relative to window")

    *x = position.x;
    *y = position.y;
    
}

void getMouseDelta(int* dX, int* dY)
{
    *dX = mouseDeltaX;
    *dY = mouseDeltaY;
}

