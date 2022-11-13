#include "../gfx_2d.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include <third-party/stb/stb_truetype.h>



/*
   Proposal:

Try to add a way to do syntax highlighting, and abstract to an extended features 
version of GFX_Text, GFX_HighlightedText perhaps. 
     
GFX_ColorToken(text, "red", v4(255,0,0,0));
     - Add to the array of syntax highlighted text, with color specifier.

Figure out a way to tokenize the string, and color it at run-time. 

*/


//~ rectangle
torn_function GFX_Rect
GFX_CreateRect(V2F size, V2F pos)
{
    
    GFX_Rect rect = {0};
    rect.size =     size;
    rect.pos = v3(pos.x, pos.y, 0);
    TORN_Log("GFX2D: Rectangle: Creating generic primitive: Rectangle.\n");
    
    // uvs
    rect.uv.top_left     = v2(0,0);
    rect.uv.top_right    = v2(1,0);
    rect.uv.bottom_left  = v2(0,1);
    rect.uv.bottom_right = v2(1,1);
    
    // color
    GFX_SetRectColor(&rect, v4(255,255,255,255));
    
    
    u32 indices[] = {  
        0, 1, 3,   
        1, 2, 3   
    };  
    
    rect.element_buffer = GFX_CreateElementBuffer(sizeof(indices),
                                                  indices);
    
    
    return rect;
    
}
torn_function void
GFX_SetRectTexture(GFX_Rect* rectangle, GFX_Texture2D texture)
{
    rectangle->texture = texture;
}
torn_function void
GFX_DrawRect(GFX_Rect* rectangle, GFX_Renderer* renderer)
{
    
    GFX_StartDraw();
    GFX_PushElementBuffer(rectangle->element_buffer);
    if (rectangle->texture.data != 0 || rectangle->texture.framebuffer_reserved)
    {
        GFX_PushTexture(&rectangle->texture);
    }
    {
        GFX_PushNormals(rectangle->normals.top_right);
        GFX_PushColor(rectangle->color.top_right);
        GFX_PushUV(rectangle->uv.top_right);
        
        GFX_PushV2(v2(
                      rectangle->pos.x + rectangle->size.w,
                      rectangle->pos.y
                      ));
        
        
        
        GFX_PushNormals(rectangle->normals.bottom_right);
        GFX_PushColor(rectangle->color.bottom_right);
        GFX_PushUV(rectangle->uv.bottom_right);
        GFX_PushV2(v2(
                      rectangle->pos.x + rectangle->size.w,
                      rectangle->pos.y + rectangle->size.h
                      ));
        
        
        GFX_PushNormals(rectangle->normals.bottom_left);
        GFX_PushColor(rectangle->color.bottom_left);
        GFX_PushUV(rectangle->uv.bottom_left);
        GFX_PushV2(v2(
                      rectangle->pos.x,
                      rectangle->pos.y + rectangle->size.h
                      ));
        
        
        GFX_PushNormals(rectangle->normals.top_left);
        GFX_PushColor(rectangle->color.top_left);
        GFX_PushUV(rectangle->uv.top_left);
        GFX_PushV2(v2(
                      rectangle->pos.x,
                      rectangle->pos.y
                      ));
        
        GFX_EndDrawIndexed(6, GFX_Triangles);
    }
    
    
    if (rectangle->texture.data != 0 || rectangle->texture.framebuffer_reserved)
    {
        GFX_PopTexture();
    } 
}


torn_function void
GFX_SetRectColor(GFX_Rect* rectangle, V4F color)
{
    rectangle->color.top_left = color;
    rectangle->color.bottom_left = color;
    rectangle->color.top_right = color;
    rectangle->color.bottom_right = color;
    
}

torn_function void
GFX_FlipRectUV(GFX_Rect* rectangle)
{
    
    rectangle->uv.top_left     = v2(0,1);
    rectangle->uv.top_right    = v2(1,1);
    rectangle->uv.bottom_left  = v2(0,0);
    rectangle->uv.bottom_right = v2(1,0);
    
}

// text
torn_function GFX_Font
GFX_CreateFont(const u8* font_path, i32 size)
{
    GFX_Font font = {0};
    
    TORN_Log("GFX2D: Font: Creating the font (%s)\n", font_path);
    
    font.font_data = OS_ReadFile(font_path);
    font.atlas_data = MEM_ArenaAlloc(MEM_GetDefaultArena(),
                                     1024*1024);
    font.characters = MEM_ArenaAlloc(MEM_GetDefaultArena(), 
                                     sizeof(stbtt_packedchar) * '~' - ' ');
    
    
    // packing
    
    TORN_Log("GFX2D: Font: Packing the font...\n");
    
    stbtt_pack_context context;
    if (!stbtt_PackBegin(&context, font.atlas_data, 1024, 1024, 0, 1, 0))
    {
        TORN_Log("GFX2D: Font: Failed to load font.\n");
        return font;
    }
    
    
    stbtt_PackSetOversampling(&context, 2, 2);
    
    if (!stbtt_PackFontRange(&context, font.font_data, 0, size, ' ', '~' - ' ', font.characters))
    {
        TORN_Log("GFX2D: Font: Failed to pack font.\n");
    }
    
    stbtt_PackEnd(&context);
    TORN_Log("GFX2D: Font: Packed the font and finished!\n");
    TORN_Log("GFX2D: Font: Creating the texture!\n");
    
    
    font.glyphs = GFX_LoadDataTexture2D(font.atlas_data,
                                        v2i(1024,1024),
                                        GFX_Format_RGBA_Internal_RED);
    
    u32 indices[] = {  
        0, 1, 2,   
        0, 2, 3   
    };  
    
    font.element_buffer = GFX_CreateElementBuffer(sizeof(indices), indices);
    font.size = size;
    
    return font;
}


torn_function GFX_Text
GFX_CreateText(u8* text,  V2F pos, GFX_Font font)
{
    GFX_Text gfx_text = {0};
    TORN_Log("GFX2D: Text: Creating text with the message: %s\n", text );
    gfx_text.text = text;
    gfx_text.font = font;
    gfx_text.pos  = pos;
    
    gfx_text.color = v4(255,255,255,255);
    return gfx_text;
}

torn_function void
GFX_DrawText(GFX_Text* text, GFX_Renderer* renderer)
{
    V2F offset = v2(0,0);
    i32 text_length = 0;
    while (text->text[text_length++] != '\0');
    
    
    
    for (i32 i = 0; i < text_length; i++){
        stbtt_aligned_quad quad;
        stbtt_GetPackedQuad(text->font.characters,
                            1024,
                            1024,
                            text->text[i]-  ' ',
                            &offset.x,
                            &offset.y,
                            &quad,
                            0);
        
        offset.x += text->h_spacing;
        
        switch (text->text[i])
        {
            case ('\n'):
            {
                offset.x = 0;
                offset.y += text->font.size + text->v_spacing;
            } break;
            
            case ('\t'):
            {
                offset.x += text->font.size;
            } break;
            
            default:
            {
                
                GFX_StartDraw();
                {
                    GFX_PushElementBuffer(text->font.element_buffer);
                    
                    GFX_PushTexture(&text->font.glyphs);
                    {
                        GFX_PushColor(text->color);
                        GFX_PushUV(v2(quad.s0, quad.t0));
                        GFX_PushV2(v2(quad.x0+text->pos.x, quad.y0 + text->pos.y)) ;
                        GFX_PushUV(v2(quad.s0, quad.t1));
                        GFX_PushV2(v2(quad.x0+text->pos.x, quad.y1 + text->pos.y ));
                        GFX_PushUV(v2(quad.s1, quad.t1));
                        GFX_PushV2(v2(quad.x1+text->pos.x, quad.y1 + text->pos.y ));
                        GFX_PushUV(v2(quad.s1, quad.t0));
                        GFX_PushV2(v2(quad.x1+text->pos.x, quad.y0 + text->pos.y ));
                    }
                    
                }
                GFX_EndDrawIndexed(6, GFX_Triangles);
            } break;
        }
        
        
        
    }
    GFX_PopTexture();
}