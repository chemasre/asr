#ifndef _SYSTEM_HPP_
#define _SYSTEM_HPP_

#include <common_system_header.hpp>

int approximately(float x, float y);
float clamp01(float x);

void wait(int millis);

int randomRange(int min, int max);

void beep(int frequency, int millis);
void initSystem();

#endif