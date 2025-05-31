#ifndef _LOG_HPP_
#define _LOG_HPP_

#include <common_header.hpp>

void logMessageVar(const char* name, int n);
void logMessageVar(const char* name, float n);
void logMessage(const char* text);
void logWarning(const char* text);
void logError(const char* text);

void beep(int frequency, int millis);
void initLog();
void finishLog();

#endif