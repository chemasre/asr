#include "input.hpp"

int isKeyPressed(char k)
{
    return GetKeyState(k) & 0x8000;
}
