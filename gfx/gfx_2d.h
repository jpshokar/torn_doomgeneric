#ifndef GFX_2D_H
#define GFX_2D_H
#include <third-party/stb/stb_truetype.h>
#include <base/base.h>
#include "gfx.h"


// The High Level Rendering API (2D)
// After the implementation of the low level rendering api, you will be able to use 
// high level rendering api as a way to interface the low level rendering api.
// This provides a really platform-agonsitic way of interfacing with the operating system and the graphics.


//~ rectangle
typedef struct GFX_Rect GFX_Rect;
struct GFX_Rect
{
    V3F pos;
    V2F size;
    
    
    struct {
        V2F top_left;
        V2F bottom_left;
        V2F top_right;
        V2F bottom_right;
    } uv;
    struct {
        V3F top_left;
        V3F bottom_left;
        V3F top_right;
        V3F bottom_right;
    } normals;
    struct {
        V4F top_left;
        V4F bottom_left;
        V4F top_right;
        V4F bottom_right;
    } color;
    
    GFX_ElementBuffer element_buffer;
    
    
    GFX_Texture2D texture;
};


torn_function GFX_Rect GFX_CreateRect(V2F size, V2F position);
torn_function void GFX_SetRectTexture(GFX_Rect* rectangle, GFX_Texture2D texture);
torn_function void GFX_SetRectColor(GFX_Rect* rectangle, V4F color);
torn_function void GFX_DrawRect(GFX_Rect* rectangle, GFX_Renderer* renderer);
torn_function void GFX_FlipRectUV(GFX_Rect* rectangle);

//~ text

// GFX_Font is a pre-loaded font object, so you don't have to keep reloading the font faces everytime you make a new text.

typedef struct GFX_Font GFX_Font;
struct GFX_Font
{
    const u8* ttf_file;
    const u8* font_data;
    u8* atlas_data;
    stbtt_packedchar* characters;
    GFX_Texture2D glyphs;
    GFX_ElementBuffer element_buffer;
    i32 size;
};


typedef struct GFX_Text GFX_Text;
struct GFX_Text
{
    u8* text; 
    GFX_Font font;
    V4F color;
    V2F pos;
    i32 v_spacing;
    i32 h_spacing;
    
};



torn_function GFX_Font GFX_CreateFont(const u8* font_path, i32 size);
torn_function GFX_Text GFX_CreateText(u8* text, V2F pos, GFX_Font font);
torn_function void GFX_DrawText(GFX_Text* text, GFX_Renderer* renderer);








#endif
