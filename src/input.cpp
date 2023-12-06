#include <input.hpp>

int isKeyPressed(int k)
{
    return GetKeyState(k) & 0x8000;
}


int isKeyDown(int k)
{
    int s = GetAsyncKeyState(k);
    return  (s & 0x8000) && (s & 0x0001);
}