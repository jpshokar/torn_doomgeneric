#ifndef WIDGETS_H
#define WIDGETS_H

#include <base/base.h>
#include <gfx/gfx_2d.h>
#include <gfx/gfx.h>


typedef union UI_Signal UI_Signal;
union UI_Signal
{
    b32 clicked;
    
};


typedef struct UI_Button UI_Button;
struct UI_Button
{
    struct {
        GFX_Rect button;
        GFX_Text text;
        GFX_Font font;
    } objects;
    
    V2F pos;
    V2F size;
    UI_Signal signal;
    u8* text;
};


torn_function UI_Button UI_CreateButton(GFX_Font font, u8* text, V2F size, V2F pos);
torn_function void UI_SetButtonPos(UI_Button* button, V2F pos);
torn_function UI_Signal UI_GetButtonSignal(UI_Button* button, OS_App* app);
torn_function void UI_UpdateButton(UI_Button* button, OS_App* app);
torn_function void      UI_DrawButton(UI_Button* button, GFX_Renderer* renderer);
torn_function b32       UI_PointInRect(V2F point, GFX_Rect rectangle);


#endif