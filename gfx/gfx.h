#ifndef GFX_H
#define GFX_H
#include <base/base.h>
#include <os/os.h>




// The Low Level Rendering API
// TODO: Camera -> Basic 2D/3D Camera manuplation.

//~ Vertex
//////////////////////////////////////////////////////

typedef struct GFX_Vertex GFX_Vertex;
struct GFX_Vertex
{
    V3F position;
    V4F color;
    V2F uv;
    V3F normals;
};


//~ Vertex Buffer
//////////////////////////////////////////////////////
typedef struct GFX_VertexBuffer GFX_VertexBuffer;
struct GFX_VertexBuffer 
{
    i32 id;
    void* handle;
    
};
torn_function GFX_VertexBuffer GFX_CreateVertexBuffer(i32 size);
torn_function void GFX_PushVertexBuffer(GFX_VertexBuffer* buffer);
torn_function void GFX_PopVertexBuffer(void);
torn_function void GFX_AddVertexBuffer(i32 offset, i32 size, GFX_Vertex vertices);



//~ Element Buffer
//////////////////////////////////////////////////////
typedef struct GFX_ElementBuffer GFX_ElementBuffer;
struct GFX_ElementBuffer
{
    i32 id;
    void* handle;
};
// NOTE:
// GFX_ElementBuffer is not batched!
// This is because GFX_ElementBuffers will be the same, and will be passed to the GPU once. 
// Therefore, the most pragmatic approach is just having a non-batched interface for element buffers.
// Also, you should be reminded of that you should use the GFX_EndDrawIndexed() function to draw with the GFX_ElementBuffer objects.

torn_function GFX_ElementBuffer GFX_CreateElementBuffer(i32 size, u32* indices);
torn_function void  GFX_PushElementBuffer(GFX_ElementBuffer buffer);
torn_function void  GFX_PopElementBuffer(GFX_ElementBuffer buffer);


//~ Shaders
//////////////////////////////////////////////////////
typedef struct GFX_Shader GFX_Shader;
struct GFX_Shader
{
    u8* vertex_shader;
    u8* fragment_shader;
    i32 program;
    
    // if there isn't an error, then it will produce a 0.
    u8* vertex_error;
    u8* fragment_error;
    u8* program_error;
    
    void* handle;
};

torn_function GFX_Shader GFX_CreateShader(u8* vertex_shader,
                                          u8* fragment_shader);
torn_function void GFX_PushShader(GFX_Shader shader);
torn_function void GFX_PopShader(void);
torn_function i32 GFX_ShaderFailed(GFX_Shader shader);
torn_function void GFX_SetTextureUnit(i32 unit);
/* 
*
*  The only thing that isn't uniform in this shader-api is the shader language, hence the multiple
*  shader functions. 
*  
*   - GLSL for OpenGL
*   - HLSL for DirectX.
*/


/*
*
* GLSL
*
*/ 
torn_function void GFX_GLSLShader1f(GFX_Shader* shader, const u8* location,  r32 v0);
torn_function void GFX_GLSLShader2f(GFX_Shader* shader, const u8* location,  r32 v0, r32 v1);
torn_function void GFX_GLSLShader3f(GFX_Shader* shader, const u8* location,  r32 v0, r32 v1, r32 v2);
torn_function void GFX_GLSLShader4f(GFX_Shader* shader, const u8* location,  r32 v0, r32 v1, r32 v2, r32 v3);
torn_function void GFX_GLSLShader1i(GFX_Shader* shader, const u8* location,  i32 v0);
torn_function void GFX_GLSLShader2i(GFX_Shader* shader, const u8* location,  i32 v0, i32 v1);
torn_function void GFX_GLSLShader3i(GFX_Shader* shader, const u8* location,  i32 v0, i32 v1, i32 v2);
torn_function void GFX_GLSLShader4i(GFX_Shader* shader, const u8* location,  i32 v0, i32 v1, i32 v2, i32 v3);
torn_function void GFX_GLSLShader1ui(GFX_Shader* shader, const u8* location,  u32 v0);
torn_function void GFX_GLSLShader2ui(GFX_Shader* shader, const u8* location,  u32 v0, u32 v1);
torn_function void GFX_GLSLShader3ui(GFX_Shader* shader, const u8* location,  u32 v0, u32 v1, u32 v2);
torn_function void GFX_GLSLShader4ui(GFX_Shader* shader, const u8* location,  u32 v0, u32 v1, u32 v2, u32 v3);
torn_function void GFX_GLSLShader1fv(GFX_Shader* shader, const u8* location,  u32 count, const r32 *value);
torn_function void GFX_GLSLShader2fv(GFX_Shader* shader, const u8* location,  u32 count, const r32 *value);
torn_function void GFX_GLSLShader3fv(GFX_Shader* shader, const u8* location,  u32 count, const r32 *value);
torn_function void GFX_GLSLShader4fv(GFX_Shader* shader, const u8* location,  u32 count, const r32 *value);
torn_function void GFX_GLSLShader1iv(GFX_Shader* shader, const u8* location,  u32 count, const i32 *value);
torn_function void GFX_GLSLShader2iv(GFX_Shader* shader, const u8* location,  u32 count, const i32 *value);
torn_function void GFX_GLSLShader3iv(GFX_Shader* shader, const u8* location, u32 count,const i32 *value);
torn_function void GFX_GLSLShader4iv(GFX_Shader* shader, const u8* location, u32 count,const i32 *value);
torn_function void GFX_GLSLShader1uiv(GFX_Shader* shader, const u8* location, u32 count,const u32 *value);
torn_function void GFX_GLSLShader2uiv(GFX_Shader* shader, const u8* location, u32 count,const u32 *value);
torn_function void GFX_GLSLShader3uiv(GFX_Shader* shader, const u8* location, u32 count,const u32 *value);
torn_function void GFX_GLSLShader4uiv(GFX_Shader* shader, const u8* location, u32 count,const u32 *value);
torn_function void GFX_GLSLShaderMatrix2fv(GFX_Shader* shader, const u8* location, u32 count,b32 transpose,const r32 *value);
torn_function void GFX_GLSLShaderMatrix3fv(GFX_Shader* shader, const u8* location, u32 count,b32 transpose,const r32 *value);
torn_function void GFX_GLSLShaderMatrix4fv(GFX_Shader* shader, const u8* location, u32 count,b32 transpose,const r32 *value);
torn_function void GFX_GLSLShaderMatrix2x3fv(GFX_Shader* shader, const u8* location, u32 count,b32 transpose,const r32 *value);
torn_function void GFX_GLSLShaderMatrix3x2fv(GFX_Shader* shader, const u8* location, u32 count,b32 transpose,const r32 *value);
torn_function void GFX_GLSLShaderMatrix2x4fv(GFX_Shader* shader, const u8* location, u32 count,b32 transpose,const r32 *value);
torn_function void GFX_GLSLShaderMatrix4x2fv(GFX_Shader* shader, const u8* location, u32 count,b32 transpose,const r32 *value);
torn_function void GFX_GLSLShaderMatrix3x4fv(GFX_Shader* shader, const u8* location, u32 count,b32 transpose,const r32 *value);
torn_function void GFX_GLSLShaderMatrix4x3fv(GFX_Shader* shader, const u8* location, u32 count,b32 transpose,const r32 *value);



#ifdef BACKEND_D3D11
/*
*
* HLSL
*
*/ 
torn_function void
GFX_HLSLShaderCBuffer(GFX_Shader* shader,i32 slot, i32 size, void* data);
#endif

//~ Vertex Array
//////////////////////////////////////////////////////
typedef enum GFX_VertexArrayFlags GFX_VertexArrayFlags;
enum GFX_VertexArrayFlags
{
    GFX_VertexArray_Position = 1,
    GFX_VertexArray_Color    = 2,
    GFX_VertexArray_Uv       = 3,
    GFX_VertexArray_Normals  = 4
};
typedef struct GFX_VertexArray GFX_VertexArray;
struct GFX_VertexArray
{
    i32 id;
    GFX_VertexArrayFlags flags;
    void* handle;
};

torn_function GFX_VertexArray GFX_CreateVertexArray(void);
torn_function void GFX_SetVertexArrayFormat(GFX_Shader* shader, GFX_VertexArray* array, GFX_VertexArrayFlags flags);
torn_function void GFX_PopVertexArray(void);
torn_function void GFX_PushVertexArray(GFX_VertexArray* array);


//~ Texture
//////////////////////////////////////////////////////
typedef enum GFX_ColorFormat GFX_ColorFormat;
enum GFX_ColorFormat
{
    GFX_Format_RGBA,
    GFX_Format_RGB,
    GFX_Format_RGB888,
    GFX_Format_BGR,
    GFX_Format_RGBA_Internal_RED, // _Internal_RED refers to using the red color channel. 
    GFX_Format_RGB_Internal_RED // _Internal_RED refers to using the RED color channel. 
};

typedef struct GFX_Texture2D GFX_Texture2D;
struct GFX_Texture2D
{
    void* path;
    i32 id;
    V2I size;
    u8* data;
    i32 channels;
    
    // reserved
    b32 framebuffer_reserved;
    void* handle;
    
};

torn_function GFX_Texture2D GFX_GenTexture2D(void);
torn_function void GFX_UpdateTexture2D(GFX_Texture2D* texture, u8* data, V2I size, GFX_ColorFormat format);
torn_function GFX_Texture2D GFX_LoadDataTexture2D(u8* data, V2I size, GFX_ColorFormat format);
torn_function GFX_Texture2D GFX_LoadTexture2D( u8* path);

//~ Renderer
//////////////////////////////////////////////////////
typedef struct GFX_Renderer GFX_Renderer;
struct GFX_Renderer
{
    V2I* window_size;
    GFX_Shader default_shader;
    GFX_VertexArray vertex_array;
    GFX_VertexBuffer vertex_buffer;
    GFX_Texture2D default_texture;
    
    void* handle;
    // States:
    V4F color;
    V2F uv;
    V3F normals;
    i32 offset;
    V4F clear_color;
    i32 vsync; // You can enable this yourself. So far, it's supported on D3D11.
};

torn_function GFX_Renderer* GFX_CreateRenderer(OS_App* app);
torn_function GFX_Renderer* GFX_GetActiveRenderer(void);
torn_function void GFX_Clear(GFX_Renderer* renderer);
torn_function void GFX_Present(GFX_Renderer* renderer);
torn_function void GFX_Resize(V2I pos, V2I size);
torn_function void GFX_SetClearColor(V4F color);
torn_function void GFX_LoadMVP(GFX_Shader* shader, GFX_Renderer* renderer);

torn_function void GFX_ShaderLoadMeta(GFX_Shader* shader, GFX_Renderer* renderer);
torn_function void GFX_ConfigureShader(GFX_Shader* shader, GFX_Renderer* renderer);


//~ IM-Based Rendering
//////////////////////////////////////////////////////
typedef enum GFX_Primitive GFX_Primitive;
enum GFX_Primitive
{
    GFX_Triangles = 1,
    GFX_TriangleFan = 2,
    GFX_TriangleStrip= 3
};

torn_function void GFX_PushRenderer(GFX_Renderer* renderer);
torn_function void GFX_StartDraw(void);
torn_function void GFX_PushTexture(GFX_Texture2D* texture);
torn_function void GFX_PushV3(V3F v_pos);
torn_function void GFX_PushV2(V2F v_pos);
torn_function void GFX_PushColor(V4F v_color);
torn_function void GFX_PushUV(V2F uv);
torn_function void GFX_PushNormals(V3F v_normals);
torn_function void GFX_PopTexture(void);
torn_function void GFX_EndDraw(GFX_Primitive primitive);
torn_function void GFX_EndDrawIndexed(i32 indices, GFX_Primitive primitive);



//~ Blending
//////////////////////////////////////////////////////

typedef enum GFX_BlendFactor GFX_BlendFactor;
enum GFX_BlendFactor
{
    GFX_ZERO = 0,
    GFX_ONE = 1,
    GFX_SRC_COLOR = 2,
    GFX_ONE_MINUS_SRC_COLOR = 3,
    GFX_DST_COLOR = 4,
    GFX_ONE_MINUS_DST_COLOR = 5,
    GFX_SRC_ALPHA =6,
    GFX_ONE_MINUS_SRC_ALPHA = 7,
    GFX_DST_ALPHA = 8,
    GFX_ONE_MINUS_DST_ALPHA = 9,
    GFX_CONSTANT_COLOR = 10,
    GFX_ONE_MINUS_CONSTANT_COLOR = 11, 
    GFX_CONSTANT_ALPHA = 12,
    GFX_ONE_MINUS_CONSTANT_ALPHA= 13,
    GFX_SRC_ALPHA_SATURATE = 14,
    GFX_SRC1_COLOR = 15,
    GFX_ONE_MINUS_SRC1_COLOR = 16,
    GFX_SRC1_ALPHA = 17,
    GFX_ONE_MINUS_SRC1_ALPHA = 18 
};

torn_function void GFX_Blend(GFX_BlendFactor blend_factor1, GFX_BlendFactor blend_factor2);
//~ Framebuffers
//////////////////////////////////////////////////////
typedef struct GFX_Framebuffer GFX_Framebuffer;
struct GFX_Framebuffer
{
    GFX_Texture2D texture;
    u32 id;
    u32 rbo_id;
};

torn_function GFX_Framebuffer GFX_CreateFramebuffer(GFX_Renderer* renderer, GFX_ColorFormat color_format);
torn_function void GFX_PushFramebuffer(GFX_Framebuffer framebuffer);
torn_function void GFX_PopFramebuffer(void);

//~ Misc 
//////////////////////////////////////////////////////
torn_function const i8* GFX_GetBackendVersion(void);
torn_function const i8* GFX_GetBackend(void);
torn_function const i8* GFX_GetGPUVendor(void);
torn_function const i8* GFX_GetGPUName(void);

#endif 

