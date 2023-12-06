#include <input.hpp>

int isKeyPressed(int k)
{
    return GetKeyState(k) & 0x8000;
}
