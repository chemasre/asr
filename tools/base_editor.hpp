#ifndef _COMMONEDITOR_HPP_
#define _COMMONEDITOR_HPP_

#include <stdio.h>
#include <stdlib.h>

#include <input.hpp>
#include <menu.hpp>
#include <ui.hpp>
#include <sprites.hpp>
#include <system.hpp>
#include <log.hpp>
#include "resource.h"

#define TITLE_STRING_SIZE 80
#define TITLE_STRING_PATTERN "%s v%d.%d.%d by %s"

#define COLOR_SELECTED MAKE_COLOR(255,255,255)
#define COLOR_UNSELECTED MAKE_COLOR(127,127,127)
#define COLOR_ERROR MAKE_COLOR(255,0,0)

#define SLOT_PER_PAGE_COUNT 16
#define SLOT_PAGES_COUNT 16
#define SLOTS_COUNT (SLOT_PER_PAGE_COUNT * SLOT_PAGES_COUNT)

// #define CELLS_WIDTH 80
// #define CELLS_HEIGHT 44

extern char hexaCharacters[];

extern int slotSelectorEnabled;
extern int selectedSlot;
extern int selectedSlotPage;

extern int cursorCellX;
extern int cursorCellY;

extern int mouseLeftPressed;
extern int mouseRightPressed;
extern int mouseLeftDown;

extern int cellsWidth;
extern int cellsHeight;

void baseEditorInit(int cellsWidth, int cellsHeight);
void baseEditorUpdate();
void baseEditorDraw();
void baseEditorDrawCursor();
void baseEditorFinish();

// Utils

int isInsideRect(int pX, int pY, int rectX, int rectY, int rectW, int rectH);

#endif