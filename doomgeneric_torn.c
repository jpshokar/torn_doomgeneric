#include "doomkeys.h"
#include "doomgeneric.h"
#include <stdio.h>
#include <torn.h>
#include <ui/ui_console.h>
#include <windows.h>

OS_App* window = 0;
GFX_Renderer* renderer = 0;
GFX_Texture2D texture;
GFX_Rect texture_rect;
UI_Console console;

#define KEYQUEUE_SIZE 16

static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;

static unsigned char convertToDoomKey(unsigned int key){
    switch (key)
	{
        case VK_RETURN:
		key = KEY_ENTER;
		break;
        case VK_ESCAPE:
		key = KEY_ESCAPE;
		break;
        case VK_LEFT:
		key = KEY_LEFTARROW;
		break;
        case VK_RIGHT:
		key = KEY_RIGHTARROW;
		break;
        case VK_UP:
		key = KEY_UPARROW;
		break;
        case VK_DOWN:
		key = KEY_DOWNARROW;
		break;
        case VK_CONTROL:
		key = KEY_FIRE;
		break;
        case VK_SPACE:
		key = KEY_USE;
		break;
        case VK_SHIFT:
		key = KEY_RSHIFT;
		break;
        default:
		key = tolower(key);
		break;
	}
    
	return key;
    
}
// TODO: Sound
static void addKeyToQueue(int pressed, unsigned int keyCode){
    unsigned char key = convertToDoomKey(keyCode);
    
    unsigned short keyData = (pressed << 8) | key;
    
    s_KeyQueue[s_KeyQueueWriteIndex] = keyData;
    s_KeyQueueWriteIndex++;
    s_KeyQueueWriteIndex %= KEYQUEUE_SIZE;
}

void key_pressed(u8 key)
{
    
    addKeyToQueue(1, key);
    
}

void key_released(u8 key)
{
    addKeyToQueue(0, key);
}
void DG_Init()
{
    TORN_Init();
    window = OS_ConstructApp("Doomgeneric - Torn",
                             OS_AppDefault,
                             OS_AppDefault,
                             OS_AppStyleDefault);
    OS_MessageBox(0, "TORN: This is DOOM written ontop of TORN", "TORN DOOM", OS_MessageBox_Info);
    TORN_Log("Ignoring specified (%d, %d) in favor for (%d,%d)\n", DOOMGENERIC_RESX,
             DOOMGENERIC_RESY, window->size.w, window->size.h);
    window->key_pressed = key_pressed;
    window->key_released = key_released;
    renderer = GFX_CreateRenderer(window);
    
    GFX_Clear(renderer);
    
    texture_rect = GFX_CreateRect(v2(window->size.w, window->size.h), v2(0,0));
    texture = GFX_GenTexture2D();
    
    console = UI_CreateConsole(window, GFX_CreateFont("font.ttf", 30));
    UI_ConsolePrintLn(&console, "Doomgeneric <GFX_Renderer - from TORN>");
}

void DG_DrawFrame()
{
    
    GFX_Clear(renderer);
    GFX_UpdateTexture2D(&texture,
                        (uint32_t*)DG_ScreenBuffer,
                        v2i(DOOMGENERIC_RESX,
                            DOOMGENERIC_RESY),
                        GFX_Format_RGB888);
    
    texture_rect.texture = texture;
    if (OS_GetKeyState('X').pressed)
    {
        UI_ShowConsole(&console);
    }
    if (OS_GetKeyState('Z').pressed) UI_HideConsole(&console);
    GFX_DrawRect(&texture_rect, renderer);
    UI_UpdateConsole(&console, renderer, window);
    UI_DrawConsole(&console, renderer);
    GFX_Present(renderer);
    OS_PollEvents(window);
}

void DG_SleepMs(uint32_t ms){
    Sleep(ms);
}


uint32_t DG_GetTicksMs()
{
    return GetTickCount();
}

int DG_GetKey(int* pressed, unsigned char* doomKey)
{
    if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex){
        //key queue is empty
        return 0;
    }else{
        unsigned short keyData = s_KeyQueue[s_KeyQueueReadIndex];
        s_KeyQueueReadIndex++;
        s_KeyQueueReadIndex %= KEYQUEUE_SIZE;
        
        *pressed = keyData >> 8;
        *doomKey = keyData & 0xFF;
        
        return 1;
    }
    
    return 0;
}
void DG_SetWindowTitle(const char * title)
{
    if (window != NULL){
        OS_SetWindowTitle(window, title);
    }
}
