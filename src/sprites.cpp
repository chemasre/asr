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

SpriteCell getSpriteCell(int index, int posX, int posY, SpriteCell boundary)
{
    if(posX < 0 || posX > SPRITE_SIZE - 1 || posY < 0 || posY > SPRITE_SIZE - 1)
    {
        return boundary;
    }
    else
    {
        return sprites[index].cells[posY][posX];
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
			int o = GET_OPACITY(c.color);
			
			if(o != 0) { setScreenCell(posX - (pX - bX) + x, posY - (pY - bY) + y, c.color, c.character); }
		}
	}
}

void drawScaledSprite(int index, int posX, int posY, float scale)
{
    int pX = sprites[index].pivotX;
    int pY = sprites[index].pivotY;
	int bX = sprites[index].boundaryX;
	int bY = sprites[index].boundaryY;
	int bW = sprites[index].boundaryWidth;
	int bH = sprites[index].boundaryHeight;
    
    float pXToBX = bX - pX;
    float pYToBY = bY - pY;
    
    float scaledBX = pX + pXToBX * scale;
    float scaledBY = pY + pYToBY * scale;
    float scaledBW = bW * scale;
    float scaledBH = bH * scale;
    
    SpriteCell boundaryCell;
    boundaryCell.color = 0;
    boundaryCell.character = ' ';
    
    int usedCells[SPRITE_SIZE][SPRITE_SIZE];
    for(int y = 0; y < SPRITE_SIZE; y ++)
    {   for(int x = 0; x < SPRITE_SIZE; x ++) { usedCells[y][x] = 0; }
    }
    
    for(int y = 0; y < scaledBH; y++)
    {
        for(int x = 0; x < scaledBW; x++)
        {
            int spriteX = pX + (scaledBX + x - pX) / scale;
            int spriteY = pY + (scaledBY + y - pY) / scale;
            
            SpriteCell c = getSpriteCell(index, spriteX, spriteY, boundaryCell);
            int o = GET_OPACITY(c.color);
            
            if(o != 0)
            {                
                setScreenCell(posX + pXToBX * scale + x, posY + pYToBY * scale + y, c.color, c.character);
            }
            
        }
    }
    
    
    
}

