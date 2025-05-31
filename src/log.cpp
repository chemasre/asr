#include <log.hpp>

#define EPSILON 0.01f

FILE* logFile;
char logLine[200];

void logMessageVar(const char* name, int n)
{
    sprintf(logLine, "MESSAGE: %s: %d\n", name, n);    
    fprintf(logFile, logLine);
    fflush(logFile);
}

void logMessageVar(const char* name, float f)
{
    sprintf(logLine, "MESSAGE: %s: %f\n", name, f);    
    fprintf(logFile, logLine);
    fflush(logFile);
}

void logMessage(const char* text)
{
    sprintf(logLine, "MESSAGE: %s\n", text);
    
    fprintf(logFile, logLine);
    fflush(logFile);
}

void logWarning(const char* text)
{
    sprintf(logLine, "WARNING: %s\n", text);
    
    fprintf(logFile, logLine);
    fflush(logFile);
}

void logError(const char* text)
{
    sprintf(logLine, "ERROR: %s\n", text);
    
    fprintf(logFile, logLine);
    fflush(logFile);
}

void initLog()
{
    logFile = fopen("log.txt", "wt");

}

void finishLog()
{
    fclose(logFile);
}

