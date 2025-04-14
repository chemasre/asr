#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cbmp/cbmp.h"

#define UCHAR unsigned char

#define MAX_FILENAME_LENGTH 256
#define OUPUT_FILENAME_EXTENSION "tex"

char* inputFileName;
char outputFileName[MAX_FILENAME_LENGTH];

void inferOutputFileName()
{
    int i = strlen(inputFileName) - 1;
    int dotPos = -1;
    while(i >= 0 && dotPos == -1)
    {   if(inputFileName[i] == '.') { dotPos = i; }
        else { i--; }
    }
    
    if(dotPos == -1) { printf("ERROR: The filename must have .bmp extension"); }

    i = dotPos - 1;
    int barPos = -1;
    while(i >= 0 && barPos == -1)
    {   if(inputFileName[i] == '\\') { barPos = i; }
        else { i--; }
    }
    
    for(i = barPos + 1; i <= dotPos; i++)
    {
        outputFileName[i - (barPos + 1)] = inputFileName[i];
    }
    
    outputFileName[dotPos + 1] = '\0';
    
    strcat(outputFileName, OUPUT_FILENAME_EXTENSION);    
}

int main(int argc, char** argv)
{
    if(argc <= 1) { printf("ERROR: The path of a 24 bit bmp is required"); exit(-1); }
    
    inputFileName = argv[1];
    BMP* image = bopen(inputFileName);
    
    int width  = get_width(image);
    int height =  get_height(image);
    unsigned int depth = get_depth(image);
    
    printf("----- Input image ---------\n");
    printf(" Width: %d\n", width);
    printf(" Height: %d\n", width);
    printf(" Depth: %d\n", depth);
    printf("---------------------------\n");
    
    if(depth != 24) { printf("ERROR: Bmp depth must be 24"); exit(-1); }

    if(argc <= 2)
    {
        inferOutputFileName();
    }
    else
    {
        strcpy(outputFileName, argv[2]);
    }
    
    
    printf("Writing to %s", outputFileName);
    
    FILE* outputFile = fopen(outputFileName, "wb");
    
    fwrite(&width, sizeof(int), 1, outputFile);
    fwrite(&height, sizeof(int), 1, outputFile);
    
    for(int y = height - 1; y >= 0; y --)
    {
        for(int x = 0; x < width; x ++)
        {
            UCHAR r;
            UCHAR g;
            UCHAR b;
            
            get_pixel_rgb(image, x, y, &r, &g, &b);
            
            //float normalizedGrayscale = ((float)(r + g + b)) / 3 / 256.0f;            
            //fwrite(&normalizedGrayscale, sizeof(float), 1, outputFile);
            
            float rF = (float)(r / 255.0f);
            float gF = (float)(g / 255.0f);
            float bF = (float)(b / 255.0f);
            
            
            fwrite(&rF, sizeof(float), 1, outputFile);
            fwrite(&gF, sizeof(float), 1, outputFile);
            fwrite(&bF, sizeof(float), 1, outputFile);

        }        
    }
    
    fclose(outputFile);

    
}