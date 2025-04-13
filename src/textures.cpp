#include <textures.hpp>

float textures[MAX_TEXTURES][TEXTURE_SIZE][TEXTURE_SIZE];


void initTextures()
{
    char fileName[MAX_TEXTURE_PATH_LENGTH] = "";
    char filePath[MAX_TEXTURE_PATH_LENGTH] = "";
    
    
    for(int i = 0; i < MAX_TEXTURES; i++)
    {
        sprintf(fileName, TEXTURE_FILENAME_PATTERN, i);
        sprintf(filePath, "%s\\%s", TEXTURE_DIRECTORY,fileName);

        FILE* texFile = fopen(filePath, "rb");
        
        ASSERT(texFile != 0, "Cannot open texture")
        
        int w;
        int h;
        
        fread(&w, sizeof(int), 1, texFile);
        fread(&h, sizeof(int), 1, texFile);
        
        ASSERT(w == TEXTURE_SIZE && h == TEXTURE_SIZE, "Texture size doesn't match")
        
        for(int y = 0; y < h; y ++)
        {
            for(int x = 0; x < w; x ++)
            {
                float f;
                
                fread(&f, sizeof(float), 1, texFile);
                
                textures[i][y][x] = f;
            }
            
        }
        
        printf("Loaded texture %s\n", filePath);
        
        fclose(texFile);
    }
}


float getTextureSample(int texture, float u, float v, float boundaryDefault)
{
    if(u < 0 || u >= 1 || v < 0 || v >= 1) { return boundaryDefault; }
    else
    { return textures[texture][(int)(v * TEXTURE_SIZE)][(int)(u * TEXTURE_SIZE)]; }
}

