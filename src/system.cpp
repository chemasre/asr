#include <system.hpp>

#define EPSILON 0.01f

int approximately(float x, float y)
{
    return fabs(x - y) < EPSILON;
}


void wait(int millis)
{
    Sleep(millis);
}
