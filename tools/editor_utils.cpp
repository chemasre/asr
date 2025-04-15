#include "editor_utils.hpp"

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