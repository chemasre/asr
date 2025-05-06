#ifndef _SYSTEM_HPP_
#define _SYSTEM_HPP_

#include <common_system_header.hpp>

int approximately(float x, float y);
float clamp01(float x);

void wait(int millis);

int randomRange(int min, int max);

void logMessage(const char* text);
void logWarning(const char* text);
void logError(const char* text);

void beep(int frequency, int millis);
void initSystem();
void finishSystem();

#endif