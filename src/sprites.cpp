#include <sprites.hpp>

Sprite sprites[MAX_SPRITES];


void initSprites()
{
    char fileName[MAX_PATH_LENGTH] = "";
    char filePath[MAX_PATH_LENGTH] = "";
    
    
    for(int i = 0; i < MAX_SPRITES; i++)
    {
        sprintf(fileName, SPRITE_FILENAME_PATTERN, i);
        sprintf(filePath, "%s\\%s", SPRITE_DIRECTORY,fileName);

        FILE* sprFile = fopen(filePath, "rb");
        
        ASSERT(sprFile != 0, "Cannot open sprite")
        
        int w;
        int h;
        
        fread(&w, sizeof(int), 1, sprFile);
        fread(&h, sizeof(int), 1, sprFile);
        
        ASSERT(w == SPRITE_SIZE && h == SPRITE_SIZE, "Sprite size doesn't match")
        
        fread(&sprites[i].pivotX, sizeof(int), 1, sprFile);
        fread(&sprites[i].pivotY, sizeof(int), 1, sprFile);
        
        fread(&sprites[i].boundaryX, sizeof(int), 1, sprFile);
        fread(&sprites[i].boundaryY, sizeof(int), 1, sprFile);
        fread(&sprites[i].boundaryWidth, sizeof(int), 1, sprFile);
        fread(&sprites[i].boundaryHeight, sizeof(int), 1, sprFile);       
        
        for(int y = 0; y < h; y ++)
        {
            for(int x = 0; x < w; x ++)
            {
                fread(&sprites[i].cells[y][x].character, sizeof(char), 1, sprFile);                
                fread(&sprites[i].cells[y][x].color, sizeof(int), 1, sprFile);                
            }
            
        }
        
        printf("Loaded sprite %s\n", filePath);
        
        fclose(sprFile);
    }
	
}

void drawSprite(int index, int posX, int posY)
{
	int pX = sprites[index].pivotX;
	int pY = sprites[index].pivotY;
	int bX = sprites[index].boundaryX;
	int bY = sprites[index].boundaryY;
	int bW = sprites[index].boundaryWidth;
	int bH = sprites[index].boundaryHeight;
	
	for(int y = 0; y < bH; y++)
	{
		for(int x = 0; x < bW; x++)
		{
			SpriteCell c = sprites[index].cells[y + bY][x + bX];
			int o = GET_COLOR_A(c.color);
			
			if(o != 0) { setScreenCell(posX - (pX - bX) + x, posY - (pY - bY) + y, c.color, c.character); }
		}
	}
}

