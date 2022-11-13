#include "../gfx.h"
#include "gfx_opengl.h"
#include <third-party/glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <third-party/stb/stb_image.h>
#include <math/math.h>
#include <base/base.h>
global GFX_Renderer* g_renderer;
#include <utils/utils.h>

///~ vertex buffers
torn_function GFX_VertexBuffer
GFX_CreateVertexBuffer(i32 size)
{
    GFX_VertexBuffer vbo;
    TORN_Log("GFX: OpenGL: Created Vertex Buffer with size: %i\n", size);
    glGenBuffers(1, &vbo.id);
    GFX_PushVertexBuffer(&vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GFX_Vertex) * size, 0, GL_DYNAMIC_DRAW);
    GFX_PopVertexBuffer();
    return vbo;
}
torn_function void 
GFX_PushVertexBuffer(GFX_VertexBuffer* buffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
}

torn_function void
GFX_AddVertexBuffer(i32 offset, i32 size, GFX_Vertex vertices)
{
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, &vertices);
}
torn_function void
GFX_PopVertexBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
//~ element buffer

torn_function GFX_ElementBuffer 
GFX_CreateElementBuffer(i32 size, u32* indices)
{
    GFX_ElementBuffer buffer = {0};
    //TORN_Log("GFX: OpenGL: Created Element Buffer with size: %i\n", size);
    glGenBuffers(1, &buffer.id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    return buffer;
    
}
torn_function void
GFX_PushElementBuffer(GFX_ElementBuffer buffer)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.id);
}
torn_function void
GFX_PopElementBuffer(GFX_ElementBuffer buffer)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//~ shaders
torn_function GFX_Shader
GFX_CreateShader(u8* vertex_shader,
                 u8* fragment_shader)
{
    GFX_Shader shader = {0};
    shader.vertex_shader = vertex_shader;
    shader.fragment_shader = fragment_shader;
    
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, 0);
    glCompileShader(vs);
    
    b32 ok = 1;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        i32 vs_error_size = 0;
        glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &vs_error_size);
        shader.vertex_error = MEM_ArenaAlloc(MEM_GetDefaultArena(), 512);
        glGetShaderInfoLog(vs, vs_error_size, 0, shader.vertex_error);
    }
    
    
    GLuint ps = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(ps, 1, &fragment_shader, 0);
    glCompileShader(ps);
    glGetShaderiv(ps, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        i32 ps_error_size = 0;
        glGetShaderiv(ps, GL_INFO_LOG_LENGTH, &ps_error_size);
        shader.fragment_error = MEM_ArenaAlloc(MEM_GetDefaultArena(), ps_error_size);
        glGetShaderInfoLog(ps, ps_error_size, 0, shader.fragment_error);
    }
    
    
    
    shader.program = glCreateProgram();
    glAttachShader(shader.program, vs);
    glAttachShader(shader.program, ps);
    glLinkProgram(shader.program);
    
    glGetProgramiv(shader.program, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        i32 p_error_size = 0;
        glGetProgramiv(shader.program, GL_INFO_LOG_LENGTH, &p_error_size);
        shader.program_error = MEM_ArenaAlloc(MEM_GetDefaultArena(), p_error_size);
        glGetProgramInfoLog(shader.program, p_error_size, 0, shader.program_error);
    }
    
    
    if (shader.vertex_error)
    {
        TORN_Error(shader.vertex_error, "Vertex Shader: Compilation Error");
    }
    if (shader.fragment_error)
    {
        TORN_Error(shader.fragment_error, "Fragment Shader: Compilation Error ");
    }
    if (shader.program_error)
    {
        TORN_Error(shader.program_error, "Program: Linking Error ");
    }
    
    glDeleteShader(vs);
    glDeleteShader(ps);
    TORN_Log("GFX: OpenGL: Compiled Vertex and Fragment shaders!\n");
    return shader;
}
torn_function void
GFX_SetTextureUnit(i32 unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
}

torn_function void
GFX_PushShader(GFX_Shader shader)
{
    glUseProgram(shader.program);
}
torn_function void
GFX_PopShader(void)
{
    glUseProgram(g_renderer->default_shader.program);
}
torn_function i32
GFX_ShaderFailed(GFX_Shader shader)
{
    return ((u8)shader.vertex_error | (u8)shader.fragment_error | (u8)shader.program_error);
}

internal i32
GFX_GetShaderLocation(GFX_Shader* shader, const u8* location)
{
    glUseProgram(shader->program);
    i32 shader_location =  glGetUniformLocation(shader->program, location);
    TORN_Log("GLSL: location <%s> => %i\n", location, shader_location);
    return shader_location;
    
}



torn_function void
GFX_GLSLShader1f(GFX_Shader* shader, const u8* location, r32 v0)
{
    r32 shader_location = GFX_GetShaderLocation(shader,location);
    glUniform1f(shader_location, v0);
}
torn_function void
GFX_GLSLShader2f(GFX_Shader* shader, const u8* location, r32 v0, r32 v1)
{
    glUniform2f(GFX_GetShaderLocation(shader,location), v0, v1);
}
torn_function void
GFX_GLSLShader3f(GFX_Shader* shader, const u8* location, r32 v0, r32 v1, r32 v2)
{
    glUniform3f(GFX_GetShaderLocation(shader,location), v0, v1, v2);
}
torn_function void
GFX_GLSLShader4f(GFX_Shader* shader, const u8* location, r32 v0, r32 v1, r32 v2, r32 v3)
{
    glUniform4f(GFX_GetShaderLocation(shader,location), v0, v1, v2, v3);
}

torn_function void
GFX_GLSLShader1i(GFX_Shader* shader, const u8* location, i32 v0)
{
    glUniform1i(GFX_GetShaderLocation(shader,location), v0);
}

torn_function void
GFX_GLSLShader2i(GFX_Shader* shader, const u8* location, i32 v0, i32 v1)
{
    glUniform2i(GFX_GetShaderLocation(shader,location), v0, v1);
}

torn_function void
GFX_GLSLShader3i(GFX_Shader* shader, const u8* location, i32 v0, i32 v1, i32 v2)
{
    glUniform3i(GFX_GetShaderLocation(shader,location), v0, v1, v2);
}

torn_function void
GFX_GLSLShader4i(GFX_Shader* shader, const u8* location, i32 v0, i32 v1, i32 v2, i32 v3)
{
    glUniform4i(GFX_GetShaderLocation(shader,location), v0, v1, v2, v3);
}

torn_function void
GFX_GLSLShader1ui(GFX_Shader* shader, const u8* location, u32 v0)
{
    glUniform1ui(GFX_GetShaderLocation(shader,location), v0);
}

torn_function void
GFX_GLSLShader2ui(GFX_Shader* shader, const u8* location, u32 v0, u32 v1)
{
    glUniform2ui(GFX_GetShaderLocation(shader,location), v0, v1);
}

torn_function void
GFX_GLSLShader3ui(GFX_Shader* shader, const u8* location, u32 v0, u32 v1, u32 v2)
{
    glUniform3ui(GFX_GetShaderLocation(shader,location), v0,v1,v2);
}

torn_function void
GFX_GLSLShader4ui(GFX_Shader* shader, const u8* location, u32 v0, u32 v1, u32 v2, u32 v3)
{
    glUniform4ui(GFX_GetShaderLocation(shader,location), v0, v1, v2, v3);
}

torn_function void
GFX_GLSLShader1fv(GFX_Shader* shader, const u8* location, u32 count, const r32 *value)
{
    glUniform1fv(GFX_GetShaderLocation(shader,location), count, value);
}

torn_function void
GFX_GLSLShader2fv(GFX_Shader* shader, const u8* location, u32 count, const r32 *value)
{
    glUniform2fv(GFX_GetShaderLocation(shader,location), count, value);
}

torn_function void
GFX_GLSLShader3fv(GFX_Shader* shader, const u8* location, u32 count, const r32 *value)
{
    glUniform3fv(GFX_GetShaderLocation(shader,location), count, value);
}

torn_function void
GFX_GLSLShader4fv(GFX_Shader* shader, const u8* location, u32 count, const r32 *value)
{
    glUniform4fv(GFX_GetShaderLocation(shader,location), count, value);
}

torn_function void
GFX_GLSLShader1iv(GFX_Shader* shader, const u8* location, u32 count, const i32 *value)
{
    glUniform1iv(GFX_GetShaderLocation(shader,location), count, value);
}

torn_function void
GFX_GLSLShader2iv(GFX_Shader* shader, const u8* location, u32 count, const i32 *value)
{
    glUniform2iv(GFX_GetShaderLocation(shader,location), count, value);
}

torn_function void
GFX_GLSLShader3iv(GFX_Shader* shader, const u8* location,u32 count,const i32 *value)
{
    glUniform3iv(GFX_GetShaderLocation(shader,location), count, value);
}

torn_function void
GFX_GLSLShader4iv(GFX_Shader* shader, const u8* location,u32 count,const i32 *value)
{
    glUniform4iv(GFX_GetShaderLocation(shader,location), count, value);
}

torn_function void
GFX_GLSLShader1uiv(GFX_Shader* shader, const u8* location,u32 count,const u32 *value)
{
    glUniform1uiv(GFX_GetShaderLocation(shader,location), count, value);
}

torn_function void
GFX_GLSLShader2uiv(GFX_Shader* shader, const u8* location,u32 count,const u32 *value)
{
    glUniform2uiv(GFX_GetShaderLocation(shader,location), count, value);
}

torn_function void
GFX_GLSLShader3uiv(GFX_Shader* shader, const u8* location,u32 count,const u32 *value)
{
    glUniform3uiv(GFX_GetShaderLocation(shader,location), count, value);
}

torn_function void
GFX_GLSLShader4uiv(GFX_Shader* shader, const u8* location,u32 count,const u32 *value)
{
    glUniform4uiv(GFX_GetShaderLocation(shader,location), count, value);
}

torn_function void
GFX_GLSLShaderMatrix2fv(GFX_Shader* shader, const u8* location,u32 count,b32 transpose,const r32 *value)
{
    glUniformMatrix2fv(GFX_GetShaderLocation(shader,location), count, transpose, value);
}

torn_function void
GFX_GLSLShaderMatrix3fv(GFX_Shader* shader, const u8* location,u32 count,b32 transpose,const r32 *value)
{
    glUniformMatrix3fv(GFX_GetShaderLocation(shader,location), count, transpose, value);
}

torn_function void
GFX_GLSLShaderMatrix4fv(GFX_Shader* shader, const u8* location,u32 count,b32 transpose,const r32 *value)
{
    glUniformMatrix4fv(GFX_GetShaderLocation(shader,location), count, transpose, value);
}

torn_function void
GFX_GLSLShaderMatrix2x3fv(GFX_Shader* shader, const u8* location,u32 count,b32 transpose,const r32 *value)
{
    glUniformMatrix2x3fv(GFX_GetShaderLocation(shader,location), count, transpose, value);
}

torn_function void
GFX_GLSLShaderMatrix3x2fv(GFX_Shader* shader, const u8* location,u32 count,b32 transpose,const r32 *value)
{
    glUniformMatrix3x2fv(GFX_GetShaderLocation(shader,location), count, transpose, value);
}

torn_function void
GFX_GLSLShaderMatrix2x4fv(GFX_Shader* shader, const u8* location,u32 count,b32 transpose,const r32 *value)
{
    glUniformMatrix2x4fv(GFX_GetShaderLocation(shader,location), count, transpose, value);
}

torn_function void
GFX_GLSLShaderMatrix4x2fv(GFX_Shader* shader, const u8* location,u32 count,b32 transpose,const r32 *value)
{
    glUniformMatrix4x2fv(GFX_GetShaderLocation(shader,location), count, transpose, value);
}

torn_function void
GFX_GLSLShaderMatrix3x4fv(GFX_Shader* shader, const u8* location,u32 count,b32 transpose,const r32 *value)
{
    glUniformMatrix3x4fv(GFX_GetShaderLocation(shader,location), count, transpose, value);
}

torn_function void
GFX_GLSLShaderMatrix4x3fv(GFX_Shader* shader, const u8* location,u32 count,b32 transpose,const r32 *value)
{
    glUniformMatrix4x3fv(GFX_GetShaderLocation(shader,location), count, transpose, value);
}
torn_function void 
GFX_ShaderLoadMeta(GFX_Shader* shader, GFX_Renderer* renderer)
{
    GFX_SetTextureUnit(0);
    GFX_PushTexture(&renderer->default_texture);
    GFX_GLSLShader1i(shader, "t_texture2d", 0);
}

torn_function void
GFX_ConfigureShader(GFX_Shader* shader, GFX_Renderer* renderer)
{
    
    GFX_LoadMVP(shader, renderer);
    GFX_PushShader(*shader);
    GFX_ShaderLoadMeta(shader, renderer);
    
}
//~ vertex array

torn_function GFX_VertexArray
GFX_CreateVertexArray(void)
{
    GFX_VertexArray vao = {0};
    
    glGenVertexArrays(1, &vao.id);
    glBindVertexArray(vao.id);
    glBindVertexArray(0);
    TORN_Log("GFX: OpenGL: Created vertex array!\n");
    return vao;
    
}


torn_function void 
GFX_SetVertexArrayFormat(GFX_Shader* shader, GFX_VertexArray* array, GFX_VertexArrayFlags flags)
{
    TORN_Log("GFX: OpenGL: Applied vertex array format!\n");
    array->flags = flags;
    GFX_PushVertexArray(array);
    if (flags & GFX_VertexArray_Position)
    {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GFX_Vertex), (void*)offsetof(GFX_Vertex, position));
        glEnableVertexAttribArray(0);
    }
    if (flags & GFX_VertexArray_Color)
    {
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,  sizeof(GFX_Vertex), (void*)offsetof(GFX_Vertex, color));
        glEnableVertexAttribArray(1);
    }
    
    if (flags & GFX_VertexArray_Uv)
    {
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,  sizeof(GFX_Vertex), (void*)offsetof(GFX_Vertex, uv));
        glEnableVertexAttribArray(2);
    }
    if (flags & GFX_VertexArray_Normals)
    {
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,   sizeof(GFX_Vertex), (void*)offsetof(GFX_Vertex, normals));
        glEnableVertexAttribArray(3);
    }
    
    // GFX_PopVertexArray();
}
torn_function void 
GFX_PushVertexArray(GFX_VertexArray* array)
{
    glBindVertexArray(array->id);
}


torn_function void 
GFX_PopVertexArray(void)
{
    glBindVertexArray(0);
}

//~ renderer
torn_function GFX_Renderer*
GFX_CreateRenderer(OS_App* app)
{
    GFX_Renderer* renderer = MEM_ArenaAlloc(MEM_GetDefaultArena(), sizeof(GFX_Renderer));
    GFXGL_LoadOpenGL(app);
    
    renderer->window_size = &app->size;
    
    glViewport(0,0, renderer->window_size->w, renderer->window_size->h);
    
    //~ shader
    
    
    u8* vertex_shader =
        "#version 330\n"
        "layout (location = 0) in vec3 i_pos;\n"
        "layout (location = 1) in vec4 i_color;\n"
        "layout (location = 2) in vec2 i_uv;\n"
        "layout (location = 3) in vec4 i_normals;\n"
        "uniform mat4 orthographic;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "out vec4 v_color;\n"
        "out vec2 v_uv;\n"
        "void main()\n"
        "{\n"
        "\tv_uv = i_uv;\n"
        "\tv_color = i_color;\n"
        "\tgl_Position = orthographic * model * view * vec4(i_pos, 1);\n"
        "}\n\0";
    
    u8* fragment_shader = 
        "#version 330\n"
        "out vec4 f_color;\n"
        "in vec4 v_color;\n"
        "in vec2 v_uv;\n"
        "uniform sampler2D t_texture2d;\n"
        "void main() {\n"
        
        "\tf_color =  texture(t_texture2d, v_uv) * v_color;\n"
        "}\n\0";
    
    //~ vertex array
    renderer->vertex_array = GFX_CreateVertexArray();
    GFX_PushVertexArray(&renderer->vertex_array);
    
    //~ vertex buffer
    renderer->vertex_buffer = GFX_CreateVertexBuffer(MAX_BATCHABLE_SIZE);
    GFX_PushVertexBuffer(&renderer->vertex_buffer);
    
    // shader
    renderer->default_shader = GFX_CreateShader(vertex_shader, fragment_shader);
    
    GFX_LoadMVP(&renderer->default_shader, renderer);
    
    GFX_PushShader(renderer->default_shader);
    
    //~ format
    GFX_SetVertexArrayFormat(&renderer->default_shader, &renderer->vertex_array, 
                             GFX_VertexArray_Position |
                             GFX_VertexArray_Color    |
                             GFX_VertexArray_Uv       |
                             GFX_VertexArray_Normals  );
    
    
    
    GFX_PushRenderer(renderer);
    
    
    // Loading default white (1x1) texture
    u8 data[4] = { 255,255,255,255 };
    renderer->default_texture = GFX_LoadDataTexture2D(data, v2i(1,1), GFX_Format_RGB);
    
    // Some drivers require you to set a unit for a sampler.
    
    
    GFX_ShaderLoadMeta(&renderer->default_shader, renderer);
    TORN_Log("GFX: OpenGL: Enabling blending!\n");
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable(GL_BLEND);
    
    TORN_Log("GFX: OpenGL: Loaded OpenGL Renderer\n");
    TORN_Log("GFX: OpenGL: Version: %s\n", GFX_GetBackendVersion());
    TORN_Log("GFX: OpenGL: Vendor: %s\n", GFX_GetGPUVendor());
    TORN_Log("GFX: OpenGL: GPU name: %s\n", GFX_GetGPUName());
    
    return renderer;
}


torn_function void
GFX_LoadMVP(GFX_Shader* shader, GFX_Renderer* renderer)
{
    
    
    M_Mat4x4 orthographic_view = M_Orthographic(0,
                                                renderer->window_size->w, 
                                                renderer->window_size->h,
                                                0,
                                                -1.f,
                                                1.f);
    
    
    M_Mat4x4 model = M_Mat4D(1);
    M_Mat4x4 view =  M_Mat4D(1);
    TORN_Log("GFX: GLSL: Loading projection!\n");
    GFX_GLSLShaderMatrix4fv(shader, "orthographic", 1, 0,
                            &orthographic_view.elements[0][0]);
    
    TORN_Log("GFX: GLSL: Loading model!\n");
    GFX_GLSLShaderMatrix4fv(shader, "model", 1, 0,
                            &model.elements[0][0]);
    TORN_Log("GFX: GLSL: Loading view!\n");
    GFX_GLSLShaderMatrix4fv(shader, "view", 1, 0,
                            &view.elements[0][0]);
    
}


torn_function void
GFX_Present(GFX_Renderer* renderer)
{
    GFXGL_Present();
}

torn_function void
GFX_Resize(V2I pos, V2I size)
{
    if (g_renderer != 0) 
    {
        if ( (size.w >= 0) || (size.h >= 0))
        {
            GFX_LoadMVP(&g_renderer->default_shader, g_renderer);
            glViewport(pos.x, pos.y, size.w, size.h);
        }
    }
    
}
torn_function GFX_Renderer*
GFX_GetActiveRenderer(void)
{
    return g_renderer;
}
//~ immediate-mode rendering
torn_function void
GFX_PushRenderer(GFX_Renderer* renderer)
{
    g_renderer = renderer;
}
torn_function void 
GFX_StartDraw(void)
{
    GFX_PushColor(v4(255,255,255,255));
    GFX_PushUV(v2(1,1));
    GFX_PushNormals(v3(1,1,1));
}
torn_function void
GFX_SetClearColor(V4F color)
{
    
    g_renderer->clear_color = color;
    glClearColor(color.r/255.0f, color.g/255.0f, color.b/255.0f, color.a/255.0f);
}
torn_function void 
GFX_Clear(GFX_Renderer* renderer)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
}


torn_function void
GFX_PushV3(V3F v_pos)
{
    GFX_Vertex vertex_data = {0};
    vertex_data.position = v_pos;
    vertex_data.color =    g_renderer->color;
    vertex_data.uv =       g_renderer->uv;
    vertex_data.normals =  g_renderer->normals;
    
    GFX_AddVertexBuffer( g_renderer->offset, sizeof(vertex_data),  vertex_data);
    
    g_renderer->offset += sizeof(vertex_data); 
    
    
}
torn_function void 
GFX_PushV2(V2F v_pos)
{
    GFX_PushV3(v3(v_pos.x, v_pos.y, 0));
}
torn_function void 
GFX_PushColor(V4F v_color)
{
    g_renderer->color = v4(v_color.r / 255.0f, v_color.g / 255.0f, v_color.b / 255.0f, v_color.a / 255.0f);
}
torn_function void 
GFX_PushUV(V2F uv)
{
    g_renderer->uv = uv;
}

torn_function void
GFX_PushNormals(V3F v_normals)
{
    g_renderer->normals = v_normals;
}
torn_function void
GFX_EndDraw(GFX_Primitive primitive)
{
    switch (primitive)
    {
        case (GFX_Triangles):
        {
            glDrawArrays(GL_TRIANGLES,0, g_renderer->offset);
        } break;
        case (GFX_TriangleFan):
        {
            glDrawArrays(GL_TRIANGLE_FAN,0, g_renderer->offset );
        } break;
        case (GFX_TriangleStrip):
        {
            glDrawArrays(GL_TRIANGLE_STRIP,0, g_renderer->offset);
        } break;
        default:
        {
            glDrawArrays(GL_TRIANGLES,0, g_renderer->offset);
        } break;
    }
    g_renderer->offset = 0;
}
torn_function void
GFX_EndDrawIndexed(i32 indices, GFX_Primitive primitive)
{
    switch (primitive)
    {
        case (GFX_Triangles):
        {
            glDrawElements(GL_TRIANGLES,indices, GL_UNSIGNED_INT, 0);
        } break;
        case (GFX_TriangleFan):
        {
            glDrawElements(GL_TRIANGLE_FAN,indices, GL_UNSIGNED_INT, 0);
        } break;
        case (GFX_TriangleStrip):
        {
            glDrawElements(GL_TRIANGLE_STRIP,indices, GL_UNSIGNED_INT, 0);
        } break;
        default:
        {
            glDrawElements(GL_TRIANGLES,indices, GL_UNSIGNED_INT, 0);
        } break;
    }
    g_renderer->offset = 0;
    
}
torn_function void
GFX_PushTexture(GFX_Texture2D* texture)
{
    glBindTexture(GL_TEXTURE_2D, texture->id);
}

torn_function void
GFX_PopTexture(void)
{
    // Can't pop to ID: 0, as that will make everything transparent
    // We are sampiling a texture in our shaders, and not having a Texture2D for sampling will not draw anything to the screen.
    GFX_PushTexture(&g_renderer->default_texture);
}

//~ texture
torn_function GFX_Texture2D
GFX_GenTexture2D(void)
{
    GFX_Texture2D texture = {0};
    glGenTextures(1, &texture.id);
    TORN_Log("GFX: Generated texture.\n");
    return texture;
}
torn_function void GFX_UpdateTexture2D(GFX_Texture2D* texture, u8* data, V2I size, GFX_ColorFormat format)
{
    glBindTexture(GL_TEXTURE_2D, texture->id);
    {
        texture->data = data;
        texture->size.w = size.w;
        texture->size.h = size.h;
        
        
        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR
                        );
        
        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_MAG_FILTER,
                        GL_LINEAR
                        );
        
        switch (format)
        {
            case GFX_Format_RGBA:
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->size.w, texture->size.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);
                
            } break;
            
            case GFX_Format_RGBA_Internal_RED:
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->size.w, texture->size.h, 0, GL_RED, GL_UNSIGNED_BYTE, texture->data);
                
                GLint swizzle_mask[] = {GL_RED, GL_RED, GL_RED, GL_RED};
                glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);
                
            } break;
            
            
            case GFX_Format_RGB_Internal_RED:
            {
                
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->size.w, texture->size.h, 0, GL_RED, GL_UNSIGNED_BYTE, texture->data);
                
                GLint swizzle_mask[] = {GL_RED, GL_RED, GL_RED, GL_RED};
                glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);
                
            } break;
            
            
            case GFX_Format_RGB:
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->size.w, texture->size.h, 0, GL_RGB , GL_UNSIGNED_BYTE, texture->data);
            } break;
            case GFX_Format_RGB888:
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->size.w, texture->size.h, 0, GL_BGRA , GL_UNSIGNED_BYTE, texture->data);
            } break;
            
            case GFX_Format_BGR:
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_BGR, texture->size.w, texture->size.h, 0, GL_BGR, GL_UNSIGNED_BYTE, texture->data);
            } break;
            
        }
        
        
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    glBindTexture(GL_TEXTURE_2D, g_renderer->default_texture.id);
    
}
torn_function GFX_Texture2D 
GFX_LoadDataTexture2D(u8* data, V2I size, GFX_ColorFormat format)
{
    GFX_Texture2D texture = GFX_GenTexture2D();
    GFX_UpdateTexture2D(&texture, data, size, format);
    TORN_Log("GFX: OpenGL: Loaded data for texture\n");
    return texture;
}

torn_function GFX_Texture2D
GFX_LoadTexture2D(u8* path)
{
    GFX_Texture2D texture = {0};
    texture.path = path;
    TORN_Log("GFX: OpenGL: Attempting to load \"%s\"\n", path);
    u8* data = stbi_load(path, &texture.size.w, &texture.size.h, &texture.channels, 0);
    
    if (data == 0)
    {
        TORN_Log("GFX: OpenGL: \"%s\" was invalid! Can't load image, returing texture as it is.\n", path);
        
        return texture;
    }
    
    i32 path_size = 0;
    while(path[path_size++] != '\0');
    
    
    i32 path_size_dot = 0;
    while(path[path_size_dot++] != '.');
    char ext[4]; /* .png, .jpeg, .bmp, .jpg supported! */
    i32 ext_fill = 0;
    for (i32 i = path_size_dot;
         i < path_size;
         i++)
    {
        ext[ext_fill++] = path[i];
    }
    // assume jpg|jpeg if all queries did not succeed.
    GFX_ColorFormat format = GFX_Format_RGB;
    
    if (U_Compare(ext, "jpg") || U_Compare(ext, "jpeg")) { format = GFX_Format_RGB; }
    if (U_Compare(ext, "png")) { format = GFX_Format_RGBA; }
    if (U_Compare(ext, "bmp")) { format = GFX_Format_BGR; }
    texture = GFX_LoadDataTexture2D(data, texture.size, format);
    GFX_PushTexture(&g_renderer->default_texture);
    return texture;
}


//~ framebuffers
torn_function void
GFX_PushFramebuffer(GFX_Framebuffer framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
}

torn_function void
GFX_PopFramebuffer(void)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
torn_function GFX_Framebuffer
GFX_CreateFramebuffer(GFX_Renderer* renderer, GFX_ColorFormat color_format)
{
    GFX_Framebuffer framebuffer = {0};
    
    TORN_Log("GFX: OpenGL: Creating framebuffer with size (%d,%d).\n", renderer->window_size->w, renderer->window_size->h); 
    
    glGenFramebuffers(1, &framebuffer.id);
    
    GFX_PushFramebuffer(framebuffer);
    framebuffer.texture = GFX_LoadDataTexture2D(0,
                                                *renderer->window_size,
                                                color_format);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer.texture.id,0);
    framebuffer.texture.framebuffer_reserved = 1;
    
    
    
#if 0 
    TORN_Log("GFX: OpenGL: Creating a renderbuffer!\n");
    u32 rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(
                          GL_RENDERBUFFER,
                          GL_DEPTH24_STENCIL8,
                          renderer->window_size->w, 
                          renderer->window_size->h
                          );
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,  framebuffer.id);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER,
                              rbo);
    
    framebuffer.rbo_id = rbo;
    TORN_Log("GFX: OpenGL: Done creating a renderbuffer!\n");
#endif
    
    
    
    
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        TORN_Log("GFX: OpenGL: Framebuffer was not complete.\n");
    TORN_Log("GFX: OpenGL: Framebuffer Status: %i\n",glCheckFramebufferStatus(GL_FRAMEBUFFER));
    
    GFX_PopFramebuffer();
    
    TORN_Log("GFX: OpenGL: Created framebuffer!\n");
    
    
    
    
    return framebuffer;
    
}

//~ Blending
///////////////////////////////////////////////////

GLenum 
GFX_GLBlendFactor(GFX_BlendFactor factor)
{
    switch(factor)
    {
        case 0: {
            return GL_ZERO; 
        } break;
        
        case 1: {
            return GL_ONE;
            
        } break;
        
        case 2: {
            return GL_SRC_COLOR;
        } break;
        
        case  3: {
            return GL_ONE_MINUS_SRC_COLOR;
        } break;
        
        case 4: {
            return GL_DST_COLOR;
        } break;
        
        case 5 : {
            return GL_ONE_MINUS_DST_COLOR;
        } break;
        
        case 6: {
            return GL_SRC_ALPHA;
        } break;
        
        case 7: {
            return GL_ONE_MINUS_SRC_ALPHA;
        } break;
        
        case 8 : {
            return GL_DST_ALPHA;
        } break;
        
        case 9: {
            return GL_ONE_MINUS_DST_ALPHA;
        } break;
        
        case 10: {
            return GL_CONSTANT_COLOR;
        } break;
        
        case 11: {
            return GL_ONE_MINUS_CONSTANT_COLOR;
        } break;
        
        case 12: {
            return GL_CONSTANT_ALPHA;
        } break;
        
        case 13: {
            return GL_ONE_MINUS_CONSTANT_COLOR;
        } break;
        
        case 14: {
            return GL_SRC_ALPHA_SATURATE;
        } break;
        
        case 15: {
            return GL_SRC1_COLOR;
        } break;
        
        case 16: {
            return GL_ONE_MINUS_SRC1_COLOR;
        } break;
        
        case 17: {
            return GL_SRC1_ALPHA;
        } break;
        
        case 18: {
            return GL_ONE_MINUS_SRC1_ALPHA;
        } break;
    };
    return 0;
}

torn_function void
GFX_Blend(GFX_BlendFactor blend_factor1, GFX_BlendFactor blend_factor2)
{
    glBlendFunc(GFX_GLBlendFactor(blend_factor1), GFX_GLBlendFactor(blend_factor2));
}



//~ misc
torn_function const i8*
GFX_GetBackendVersion(void)
{
    return glGetString(GL_VERSION);
}

torn_function const i8*
GFX_GetGPUVendor(void)
{
    return glGetString(GL_VENDOR);
}

torn_function const i8*
GFX_GetGPUName(void)
{
    return glGetString(GL_RENDERER);
}


torn_function const i8*
GFX_GetBackend(void)
{
    return "OpenGL";
}

