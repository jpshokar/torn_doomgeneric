#include <ui/widgets.h>



torn_function UI_Button 
UI_CreateButton(GFX_Font font, u8* text, V2F size, V2F pos)
{
    UI_Button button = {0};
    
    button.objects.button = GFX_CreateRect(size,pos);
    button.objects.font   = font;
    button.pos = pos;
    button.size = size;
    
    button.objects.text   = GFX_CreateText(text, 
                                           
                                           v2(
                                              (pos.x + ((size.w - (U_GetTextLength(text) * font.size)/2)/ 2)) + (font.size/2) 
                                              
                                              , 
                                              (pos.y + (size.h / 2)) + (font.size/4))
                                           
                                           , font);
    
    button.text = text;
    
    return button;
}

torn_function b32 
UI_PointInRect(V2F point, GFX_Rect rectangle)
{
    return ((point.y <= rectangle.size.y + rectangle.pos.y) &&
            (point.y >=  rectangle.pos.y) &&
            (point.x <= rectangle.size.x + rectangle.pos.x) &&
            (point.x >=  rectangle.pos.x));
    
}

torn_function void
UI_UpdateButton(UI_Button* button, OS_App* app)
{
    
    if (UI_PointInRect(OS_GetMousePosition(app), button->objects.button))
    {
        
        {
            V4F g1 =  COLOR_BTN_G1_SELECTED ;
            V4F g2 =  COLOR_BTN_G2_SELECTED ;
            
            
            button->objects.button.color.top_left = g1;
            button->objects.button.color.top_right = g1;
            
            
            button->objects.button.color.bottom_left = g2;
            button->objects.button.color.bottom_right = g2;
            
        }
        
        if (OS_GetLeftMouseState().clicked)
        {
            {
                V4F g1 = COLOR_BTN_G1_PRESSED ;
                V4F g2 = COLOR_BTN_G2_PRESSED ;
                
                
                button->objects.button.color.top_left = g1;
                button->objects.button.color.top_right = g1;
                
                button->objects.button.color.bottom_left = g2;
                button->objects.button.color.bottom_right = g2;
            }
            button->signal.clicked = 1;
        } else
        {
            button->signal.clicked = 0;
        }
        
    } else {
        {
            V4F g1 = COLOR_BTN_G1_INACTIVE;
            V4F g2 = COLOR_BTN_G2_INACTIVE;
            
            
            button->objects.button.color.top_left = g1;
            button->objects.button.color.top_right = g1;
            
            
            button->objects.button.color.bottom_left = g2;
            button->objects.button.color.bottom_right = g2;
        }
        
        button->signal.clicked = 0;
    }
    
}
torn_function void
UI_SetButtonPos(UI_Button* button, V2F pos)
{
    button->pos = pos;
    
    V2F position = 
        v2((button->pos.x + ((button->size.w - (U_GetTextLength(button->text) * button->objects.font.size)/2)/ 2)) + (button->objects.font.size/2), 
           (button->pos.y + (button->size.h / 2)) + (button->objects.font.size/4));
    
    button->objects.button.pos = v3(pos.x, pos.y,0);
    button->objects.text.pos = v2(position.x, position.y);
    
}
torn_function 
void UI_DrawButton(UI_Button* button, GFX_Renderer* renderer)
{
    
    GFX_DrawRect(&button->objects.button, renderer);
    GFX_DrawText(&button->objects.text,    renderer);
}

torn_function UI_Signal
UI_GetButtonSignal(UI_Button* button, OS_App* app)
{
    return button->signal;
}
