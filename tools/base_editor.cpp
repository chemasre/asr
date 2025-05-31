#include "base_editor.hpp"

#define SLOT_DIGITS_COUNT 2

#define SLOT_PREVIOUSPAGE_POSITION_X ((cellsWidth - (SLOTS_WIDTH + SLOT_CHANGEPAGE_WIDTH * 2)) / 2) 
#define SLOT_PREVIOUSPAGE_POSITION_Y (SLOTS_POSITION_Y)
#define SLOT_CHANGEPAGE_WIDTH 3
#define SLOT_CHANGEPAGE_HEIGHT 1

#define SLOTS_WIDTH (SLOT_PER_PAGE_COUNT * 3)
#define SLOTS_HEIGHT 1
#define SLOTS_POSITION_X (SLOT_PREVIOUSPAGE_POSITION_X + SLOT_CHANGEPAGE_WIDTH + 1)
#define SLOTS_POSITION_Y (cellsHeight - SLOTS_HEIGHT - 1)

#define SLOT_NEXTPAGE_POSITION_X (SLOTS_POSITION_X + SLOTS_WIDTH + 1)
#define SLOT_NEXTPAGE_POSITION_Y (SLOTS_POSITION_Y)

char hexaCharacters[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

int slotSelectorEnabled;
int selectedSlot;
int selectedSlotPage;

char cursorChar;
int cursorCellX;
int cursorCellY;
int cursorColor;
int mouseLeftPressed;
int mouseRightPressed;
int mouseLeftDown;

int cellsWidth;
int cellsHeight;

int selectableAreaX;
int selectableAreaY;
int selectableAreaW;
int selectableAreaH;

void baseEditorInit(int width, int height)
{
    cursorChar = 'O';
    cursorColor = MAKE_COLOR(255, 255, 255);
    
    selectedSlot = 0;
    selectedSlotPage = 0;
    
    cellsWidth = width;
    cellsHeight = height;
    
    slotSelectorEnabled = 1;

}

void baseEditorUpdate()
{
    int cursorWindowX;
    int cursorWindowY;
    getMousePosition(&cursorWindowX, &cursorWindowY);
    
    cursorCellX = cursorWindowX / fontWidth;
    cursorCellY = cursorWindowY / fontHeight;

    mouseLeftPressed = isKeyPressed(MOUSE_LEFT) ? 1 : 0;
    mouseRightPressed = isKeyPressed(MOUSE_RIGHT) ? 1 : 0;
    mouseLeftDown = isKeyDown(MOUSE_LEFT) ? 1 : 0;
            
    if(mouseLeftPressed) { cursorChar = 'X'; }
    else { cursorChar = 'O'; }     

    if(!isKeyPressed(KEY_ALT) && slotSelectorEnabled)
    {
        if(isKeyDown(KEY_0)) { selectedSlot = selectedSlotPage * SLOT_PER_PAGE_COUNT + 0; }
        else if(isKeyDown(KEY_1)) { selectedSlot = selectedSlotPage * SLOT_PER_PAGE_COUNT + 1; }
        else if(isKeyDown(KEY_2)) { selectedSlot = selectedSlotPage * SLOT_PER_PAGE_COUNT + 2; }
        else if(isKeyDown(KEY_3)) { selectedSlot = selectedSlotPage * SLOT_PER_PAGE_COUNT + 3; }
        else if(isKeyDown(KEY_4)) { selectedSlot = selectedSlotPage * SLOT_PER_PAGE_COUNT + 4; }
        else if(isKeyDown(KEY_5)) { selectedSlot = selectedSlotPage * SLOT_PER_PAGE_COUNT + 5; }
        else if(isKeyDown(KEY_6)) { selectedSlot = selectedSlotPage * SLOT_PER_PAGE_COUNT + 6; }
        else if(isKeyDown(KEY_7)) { selectedSlot = selectedSlotPage * SLOT_PER_PAGE_COUNT + 7; }
        else if(isKeyDown(KEY_8)) { selectedSlot = selectedSlotPage * SLOT_PER_PAGE_COUNT + 8; }
        else if(isKeyDown(KEY_9)) { selectedSlot = selectedSlotPage * SLOT_PER_PAGE_COUNT + 9; }
    }
    
    if(isInsideRect(cursorCellX, cursorCellY,
                    SLOT_PREVIOUSPAGE_POSITION_X, SLOT_PREVIOUSPAGE_POSITION_Y,
                    SLOT_CHANGEPAGE_WIDTH, SLOT_CHANGEPAGE_HEIGHT) && slotSelectorEnabled)
    {
        if(mouseLeftPressed)
        {
            if(selectedSlotPage > 0) { selectedSlotPage --; }

            selectedSlot = selectedSlotPage * SLOT_PER_PAGE_COUNT + (selectedSlot % SLOT_PER_PAGE_COUNT);
        }
    }        
    else if(isInsideRect(cursorCellX, cursorCellY,
                    SLOT_NEXTPAGE_POSITION_X, SLOT_NEXTPAGE_POSITION_Y,
                    SLOT_CHANGEPAGE_WIDTH, SLOT_CHANGEPAGE_HEIGHT) && slotSelectorEnabled)
    {
        if(mouseLeftPressed)
        {
            if(selectedSlotPage < SLOT_PAGES_COUNT - 1) { selectedSlotPage ++; }
            
            selectedSlot = selectedSlotPage * SLOT_PER_PAGE_COUNT + (selectedSlot % SLOT_PER_PAGE_COUNT);
        }
    }
    else if(isInsideRect(cursorCellX, cursorCellY,
                        SLOTS_POSITION_X, SLOTS_POSITION_Y,
                        SLOTS_WIDTH, SLOTS_HEIGHT) && slotSelectorEnabled)
    {
        if(mouseLeftPressed)
        {            
            selectedSlot = selectedSlotPage * SLOT_PER_PAGE_COUNT + (cursorCellX - SLOTS_POSITION_X) / (SLOT_DIGITS_COUNT + 1); 
        }            
    }     
    
}

void baseEditorDraw()
{
    drawWindow(SLOT_PREVIOUSPAGE_POSITION_X - 1, SLOT_PREVIOUSPAGE_POSITION_Y - 1, SLOT_CHANGEPAGE_WIDTH * 2 + SLOTS_WIDTH + 4, SLOTS_HEIGHT + 2, NULL, MAKE_COLOR(255, 255, 0));

	setScreenCell(SLOT_PREVIOUSPAGE_POSITION_X + SLOT_CHANGEPAGE_WIDTH, SLOT_PREVIOUSPAGE_POSITION_Y, MAKE_COLOR(255,255,0), '|');
	setScreenCell(SLOT_NEXTPAGE_POSITION_X - 1, SLOT_NEXTPAGE_POSITION_Y, MAKE_COLOR(255,255,0), '|');
    
	drawString(slotSelectorEnabled ? MAKE_COLOR(255,255,255):COLOR_UNSELECTED, " < ", SLOT_PREVIOUSPAGE_POSITION_X, SLOT_PREVIOUSPAGE_POSITION_Y);
	drawString(slotSelectorEnabled ? MAKE_COLOR(255,255,255):COLOR_UNSELECTED, " > ", SLOT_NEXTPAGE_POSITION_X, SLOT_NEXTPAGE_POSITION_Y);
    
    for(int x = 0; x < SLOT_PER_PAGE_COUNT; x ++)
    {
        int slot = x + SLOT_PER_PAGE_COUNT * selectedSlotPage;
        
        int color;
        if(slot == selectedSlot) { color = COLOR_SELECTED; }
        else { color = COLOR_UNSELECTED; }   
        
        int number = slot;

        for(int d = 0; d < SLOT_DIGITS_COUNT; d ++)
        {
            int digit = number % 16;
			number = number / 16;
            
            setScreenCell(SLOTS_POSITION_X + x * (SLOT_DIGITS_COUNT + 1) + (SLOT_DIGITS_COUNT - d - 1), SLOTS_POSITION_Y, color, hexaCharacters[digit]);
        }

    }    
}

void baseEditorDrawCursor()
{
    setScreenCell(cursorCellX, cursorCellY, cursorColor, cursorChar);
}

void baseEditorFinish()
{
    
}

int isInsideRect(int pX, int pY, int rectX, int rectY, int rectW, int rectH)
{
    if(pX >= rectX && pX <= rectX + rectW - 1 && pY >= rectY && pY <= rectY + rectH - 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void setSelectionTopLeft(int x, int y, int* sX, int* sY, int* sW, int* sH)
{
	if(x <= *sX + *sW - 1)
	{
		*sX = x;
		*sW = *sW - (x - *sX);
		if(*sW <= 0) { printf("NEGATIVE Width"); exit(-1); }
	}
	else
	{
		*sX = x;
		*sW = 1;
	}
	
	if(y <= *sY + *sH - 1)
	{
		*sY  = y;
		*sH = *sH - (y - *sY);
		if(*sH < 0) { printf("NEGATIVE Height"); exit(-1); }
	}
	else
	{
		*sY = y;
		*sH = 1;
	}
	
	if(*sX + *sW - 1 > selectableAreaW - 1)
	{
		*sW = selectableAreaW - *sX;		
	}
	
	if(*sY + *sH - 1 > selectableAreaH - 1)
	{
		*sH = selectableAreaH - *sY;		
	}
}

void initSelectableArea(int x, int y, int w, int h)
{
    selectableAreaX = x;
    selectableAreaY = y;
    selectableAreaW = w;
    selectableAreaH = h;
}

void setSelectionBottomRight(int x, int y, int* sX, int* sY, int* sW, int* sH)
{
	*sW = x < *sX ? 1 : x - *sX + 1;
	*sH = y < *sY ? 1 : y - *sY + 1;                        

}

void intersectSelection(int &x1, int &y1, int &w1, int &h1, int x2, int y2, int w2, int h2)
{
	
}

void drawSelection(int sX, int sY, int sW, int sH, int mC)
{
	int dX = selectableAreaX;
	int dY = selectableAreaY;
	
	setScreenCell(dX + sX,          dY + sY, mC, '+');
	setScreenCell(dX + sX + sW - 1, dY + sY, mC, '+');
	setScreenCell(dX + sX,          dY + sY + sH - 1, mC, '+');
	setScreenCell(dX + sX + sW - 1, dY + sY + sH - 1, mC, '+');
	
	for(int x = 1; x - 1 < sW - 2; x ++)
	{
		setScreenCell(dX + sX + x, dY + sY, mC, '-');
		setScreenCell(dX + sX + x, dY + sY + sH - 1, mC, '-');
	}
	
	for(int y = 1; y - 1 < sH - 2; y ++)
	{
		setScreenCell(dX + sX,          dY + sY + y, mC, '|');
		setScreenCell(dX + sX + sW - 1, dY + sY + y, mC, '|');
	}

}
