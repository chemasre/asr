#include <ui.hpp>
#include <player.hpp>
#include <map.hpp>
#include <view.hpp>


#define DEFAULT_WINDOW_CORNER_TOP_LEFT '.'
#define DEFAULT_WINDOW_CORNER_TOP_RIGHT '.'
#define DEFAULT_WINDOW_CORNER_BOTTOM_LEFT '`'
#define DEFAULT_WINDOW_CORNER_BOTTOM_RIGHT '`'
#define DEFAULT_WINDOW_BORDER_HORIZONTAL '-'
#define DEFAULT_WINDOW_BORDER_VERTICAL '|'
#define DEFAULT_WINDOW_DIVIDER_TIP_LEFT '|'
#define DEFAULT_WINDOW_DIVIDER_TIP_RIGHT '|'

int windowStylesEnabled;
int textAreaColor;
int windowColor;
int windowTitleColor;
int windowCornerTopLeft = '.';
int windowCornerTopRight = '.';
int windowCornerBottomLeft = '`';
int windowCornerBottomRight = '`';
int windowBorderHorizontal = '-';
int windowBorderVertical = '|';
int windowDividerTipLeft = '|';
int windowDividerTipRight = '|';

void initUI()
{
    windowColor = MAKE_COLOR(255, 255, 255);
    windowTitleColor = MAKE_COLOR(255, 255, 255);
    windowStylesEnabled = 0;
}

void enableWindowStyles()
{
    windowStylesEnabled = 1;
}

void disableWindowStyles()
{
    windowStylesEnabled = 0;
}

void drawWindow(int windowX, int windowY, int windowWidth, int windowHeight, char title[], int color)
{
    int color1 = windowStylesEnabled ? windowColor : color;
    int color2 = windowStylesEnabled ? windowTitleColor : color;
    char cornerTL = windowStylesEnabled ? windowCornerTopLeft : DEFAULT_WINDOW_CORNER_TOP_LEFT;
    char cornerTR = windowStylesEnabled ? windowCornerTopRight : DEFAULT_WINDOW_CORNER_TOP_RIGHT;
    char cornerBL = windowStylesEnabled ? windowCornerBottomLeft : DEFAULT_WINDOW_CORNER_BOTTOM_LEFT;
    char cornerBR = windowStylesEnabled ? windowCornerBottomRight : DEFAULT_WINDOW_CORNER_BOTTOM_RIGHT;
    char borderH = windowStylesEnabled ? windowBorderHorizontal : DEFAULT_WINDOW_BORDER_HORIZONTAL;
    char borderV = windowStylesEnabled ? windowBorderVertical : DEFAULT_WINDOW_BORDER_VERTICAL;
    char dividerTipL = windowStylesEnabled ? windowDividerTipLeft : DEFAULT_WINDOW_DIVIDER_TIP_LEFT;
    char dividerTipR = windowStylesEnabled ? windowDividerTipRight : DEFAULT_WINDOW_DIVIDER_TIP_RIGHT;
    
    // Corners
    
    setScreenCell(windowX, windowY, color1, cornerTL);
    setScreenCell(windowX + windowWidth - 1, windowY, color1, cornerTR);
    setScreenCell(windowX, windowY + windowHeight - 1, color1, cornerBL);
    setScreenCell(windowX + windowWidth - 1, windowY + windowHeight - 1, color1, cornerBR);
    
    // Horizontal borders

    fillScreenArea(windowX + 1, windowY, windowWidth - 2, 1, color1, borderH);
    fillScreenArea(windowX + 1, windowY + windowHeight - 1, windowWidth - 2, 1, color1, borderH);

    int blankAreaY = windowY + 1;
    int blankAreaHeight = windowHeight - 2;
    
    if(title != NULL)
    {
        // Title borders
        
        setScreenCell(windowX, windowY + 1, color1, borderV);
        setScreenCell(windowX + windowWidth - 1, windowY + 1, color1, borderV);
        
        // Title
        
        fillScreenArea(windowX + 1, windowY + 1, windowWidth - 2, 1, color1, ' ');
        drawString(color2, title, windowX + 2, windowY + 1);
        
        // Title divider tips
        
        setScreenCell(windowX, windowY + 2, color1, dividerTipL);
        setScreenCell(windowX + windowWidth - 1, windowY + 2, color1, dividerTipR);
        
        // Title divider interior

        fillScreenArea(windowX + 1, windowY + 2, windowWidth - 2, 1, color1, borderH);
        
        blankAreaY = blankAreaY + 2;
        blankAreaHeight = blankAreaHeight - 2;
    }
    
    // Vertical borders
    
    fillScreenArea(windowX, blankAreaY, 1, blankAreaHeight, color1, borderV);
    fillScreenArea(windowX + windowWidth - 1, blankAreaY, 1, blankAreaHeight, color1, borderV);
    
    // Blank area

    fillScreenArea(windowX + 1, blankAreaY, windowWidth - 2, blankAreaHeight, color1, ' ');
    
}

void drawTextArea(int x, int y, int w, int h, const char text[], int color)
{
    int textColor = windowStylesEnabled ? textAreaColor : color;
    int textLength = strlen(text);
    int cursorX = 0;
    int cursorY = 0;
    
    int wordStart = 0;
    int wordLength = 0;
    
    while(wordStart < textLength)
    {
        // Skip spaces
        
        char c = text[wordStart];
        while(wordStart < textLength && c == ' ')
        {
            wordStart ++;            
            if(wordStart < textLength) { c = text[wordStart]; }
        }
        
        if(wordStart < textLength)
        {
            // Get word length
            int wordLength = 0;
            char c = text[wordStart + wordLength];
            while(wordStart + wordLength < textLength && c != ' ')
            {
                wordLength ++;            
                if(wordStart + wordLength < textLength) { c = text[wordStart + wordLength]; }
            }

            if(wordStart + wordLength >= textLength) { wordLength = textLength - wordStart; }
            
            // Calculate cursor position
            
            if(cursorX + wordLength - 1 >= w) { cursorY += 1; cursorX = 0; }
            
            // Draw word
            
            for(int i = 0; i < wordLength; i++)
            {
                setScreenCell(x + cursorX, y + cursorY, textColor, text[wordStart + i]);                
                cursorX ++;
            }
            
            // Draw space
            
            cursorX ++;
            if(cursorX < w - 1)
            {
                setScreenCell(x + cursorX, y + cursorY, textColor, ' ');
            }
            
            // Set initial position for next word
            
            wordStart = wordStart + wordLength;
        }
        
        
        
        
        
        
        
        
    }
}


