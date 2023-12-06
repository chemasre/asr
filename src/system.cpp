#include <system.hpp>

#define EPSILON 0.01f

int approximately(float x, float y)
{
    return fabs(x - y) < EPSILON;
}

float clamp01(float x) { return x < 0 ? 0 : x > 1 ? 1 : x; }


void wait(int millis)
{
    Sleep(millis);
}
