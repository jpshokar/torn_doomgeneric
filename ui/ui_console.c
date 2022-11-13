#include "ui_console.h"



torn_function UI_Console
UI_CreateConsole(OS_App* app, GFX_Font font)
{
    V2F padding = v2(10,10);
    UI_Console console = {
        .background = GFX_CreateRect(v2(app->size.w, app->size.h/3), v2(0,0)),
        .foreground = GFX_CreateRect(v2((app->size.w)-padding.x, (app->size.h/3)-padding.y), v2(padding.x/2,padding.y/2)),
        
        
        
    };
    
    console.font = font;
    GFX_SetRectColor(&console.background,
                     v4(20,20,20,200));
    
    GFX_SetRectColor(&console.foreground,
                     v4(0,0,0,150));
    
    
    console.arena = MEM_ArenaInit(50000);
    
    console.line.texts = MEM_ArenaAlloc(&console.arena, 
                                        sizeof(GFX_Text)*1024);
    
    console.line.text_used = 0;
    console.line.col = 0;
    
    console.scrollbar.base = GFX_CreateRect(v2(30,
                                               console.foreground.size.h),
                                            v2(console.foreground.size.w - 30,
                                               console.foreground.pos.y));
    
    console.scrollbar.button = GFX_CreateRect(v2(30,
                                                 console.foreground.size.h/4),
                                              v2(console.foreground.size.w - 30,
                                                 console.foreground.pos.y));
    
    
    
    GFX_SetRectColor(&console.scrollbar.base,
                     v4(30,30,30,255));
    
    
    
    GFX_SetRectColor(&console.scrollbar.button,
                     v4(60,60,60,255));
    
    console.line.text_used+=1;
    return console;
}
torn_function void
UI_ConsolePrintLn(UI_Console* console,  u8* text)
{
    console->line.col += 1;
    console->line.texts[console->line.text_used] = GFX_CreateText(text, v2(console->foreground.pos.x + 10,
                                                                           console->foreground.pos.y + (console->font.size * (console->line.col)) +
                                                                           console->font.size), console->font);
    console->line.text_used += 1;
}
torn_function void
UI_UpdateConsole(UI_Console* console, GFX_Renderer* renderer, OS_App* app)
{
    if (console->visiblity){
        
        console->scrollbar.button.size.h = console->scrollbar.base.size.h / console->line.text_used;
        if (UI_PointInRect(OS_GetMousePosition(app), console->scrollbar.base))
        {
            if (OS_GetLeftMouseState().clicked)
            {
                console->scrollbar.button.pos.y = OS_GetMousePosition(app).y - console->scrollbar.button.size.h/2;
                console->scrollbar.offset = 
                    0 + (100 * (console->line.text_used) - 0) * ((OS_GetMousePosition(app).y - console->scrollbar.base.pos.y) /
                                                                 console->scrollbar.base.size.y);
            }
        }
        if (console->scrollbar.clicked)
        {
            GFX_SetRectColor(&console->scrollbar.button,
                             v4(80,80,80,255));
            console->scrollbar.button.pos.y = OS_GetMousePosition(app).y - console->scrollbar.button.size.h/2;
            
            console->scrollbar.offset = 
                0 + (100 * (console->line.text_used) - 0) * ((OS_GetMousePosition(app).y - console->scrollbar.base.pos.y) /
                                                             console->scrollbar.base.size.y);
        }
        if (!OS_GetLeftMouseState().clicked)  {
            console->scrollbar.clicked = 0;
            GFX_SetRectColor(&console->scrollbar.button,
                             v4(60,60,60,255));
        }
        if (UI_PointInRect(OS_GetMousePosition(app), console->scrollbar.button))
        {
            GFX_SetRectColor(&console->scrollbar.button,
                             v4(80,80,80,255));
            if (OS_GetLeftMouseState().clicked)
            {
                console->scrollbar.button.pos.y = OS_GetMousePosition(app).y - console->scrollbar.button.size.h/2;
                console->scrollbar.offset = 
                (100 * (console->line.text_used)) * ((OS_GetMousePosition(app).y - console->scrollbar.base.pos.y) / console->scrollbar.base.size.y);
                console->scrollbar.clicked = 1;
            }
        } else if (!console->scrollbar.clicked)
            GFX_SetRectColor(&console->scrollbar.button,
                             v4(60,60,60,255));
        for (int i=0;i<console->line.text_used;i++) 
        {
            console->line.texts[i].pos.y =  (i*console->font.size) - console->scrollbar.offset;
            console->scrollbar.offset;
        }
        console->scrollbar.button.pos.y = 
            U_Clamp(console->scrollbar.button.pos.y, (console->scrollbar.base.pos.y + console->scrollbar.base.size.h) - console->scrollbar.button.size.h,
                    console->scrollbar.base.pos.y);
        
        
    }
    
    
}
torn_function void 
UI_DrawConsole(UI_Console* console, GFX_Renderer* renderer)
{
    
    if (console->visiblity){
        GFX_DrawRect(&console->background, renderer);
        GFX_DrawRect(&console->foreground, renderer);
        GFX_DrawRect(&console->scrollbar.base, renderer);
        GFX_DrawRect(&console->scrollbar.button, renderer);
        
        for (int i=0;i<console->line.text_used;i++) 
        {
            if (UI_PointInRect(console->line.texts[i].pos, 
                               console->foreground))
            {
                
                GFX_DrawText(&console->line.texts[i], renderer);
            }
        }
    }
}



torn_function void
UI_ShowConsole(UI_Console* console)
{
    console->visiblity = 1;
}
torn_function void
UI_HideConsole(UI_Console* console)
{
    console->visiblity = 0;
}

