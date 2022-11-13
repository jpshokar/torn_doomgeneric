
// Perhaps redo this? I don't know.
// #error "(D3D11) This pipeline is incomplete, awaiting a major revamp or plagued by a bug, therefore you are unable to access it."


#include "../gfx.h"
#include <os/os.h>
#include <base/base.h>
#define STB_IMAGE_IMPLEMENTATION
#include <third-party/stb/stb_image.h>
#define COBJMACROS
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <windows.h>
#pragma comment (lib, "gdi32.lib")
#pragma comment (lib, "user32.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma message ( "torn: d3d11: This pipeline is extremely experimental, and may not have the correct standard behaviour. Also, this pipeline will not work." )
#include "gfx_d3d11_handle.h"
#include <math/math.h>
//////////////////////////////////////////////////////
//~ Global for state-based programming!
global GFX_Renderer* g_renderer;
global GFX_VertexBuffer* g_buffer;
global GFX_Texture2D* g_texture2d;
//////////////////////////////////////////////////////



//~ Vertex Buffer
//////////////////////////////////////////////////////
torn_function GFX_VertexBuffer
GFX_CreateVertexBuffer(i32 size)
{
    GFX_VertexBuffer buffer = {0};
    
    buffer.handle = HANDLE_CreateVertexBuffer();
    
    GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(g_renderer->handle);
    GFXD3D11_VertexBuffer* vbhandle = HANDLE_FetchVertexBuffer(buffer.handle); // might be confused for vhandle, so it's prefixed by vb.
    
    D3D11_BUFFER_DESC bdesc = {0};
    
    bdesc.Usage = D3D11_USAGE_DYNAMIC;
    bdesc.ByteWidth = size * sizeof(GFX_Vertex);
    bdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbhandle->vertices = size;
    
    GFXD3D11_Validate(ID3D11Device_CreateBuffer(rhandle->device,
                                                &bdesc,
                                                0,
                                                &vbhandle->vertex_buffer));
    
    return buffer;
    
}
torn_function void
GFX_PushVertexBuffer(GFX_VertexBuffer* buffer)
{
    g_buffer = buffer;
}
torn_function void
GFX_PopVertexBuffer(void)
{
    g_buffer = 0; // [[possible data violation]]
}


torn_function void 
GFX_AddVertexBuffer(i32 offset, i32 size, GFX_Vertex vertices)
{
    GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(g_renderer->handle);
    GFXD3D11_VertexBuffer* vbhandle = HANDLE_FetchVertexBuffer(g_buffer->handle);
    
    
    D3D11_MAPPED_SUBRESOURCE subresource = {0};
    GFXD3D11_Validate(ID3D11DeviceContext_Map(rhandle->device_context,
                                              (ID3D11Resource*)vbhandle->vertex_buffer,
                                              0,
                                              D3D11_MAP_WRITE_DISCARD,
                                              0,
                                              &subresource));
    
    CopyMemory((u8*)subresource.pData + offset, &vertices, size);
    
    ID3D11DeviceContext_Unmap(rhandle->device_context,
                              (ID3D11Resource*)vbhandle->vertex_buffer,
                              0);
    
    
}

//~ Element Buffer
//////////////////////////////////////////////////////

torn_function GFX_ElementBuffer
GFX_CreateElementBuffer(i32 size, u32* indices)
{
    GFX_ElementBuffer element_buffer = {0};
    element_buffer.handle = HANDLE_CreateElementBuffer();
    GFXD3D11_ElementBuffer* ehandle  = HANDLE_FetchElementBuffer(element_buffer.handle);
    GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(g_renderer->handle);
    
    D3D11_BUFFER_DESC indexbd = {0};
    indexbd.ByteWidth = size;
    indexbd.Usage     = D3D11_USAGE_IMMUTABLE;
    indexbd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    
    D3D11_SUBRESOURCE_DATA subresource = {  indices };
    
    
    GFXD3D11_Validate(ID3D11Device_CreateBuffer(rhandle->device,
                                                &indexbd,
                                                &subresource,
                                                &ehandle->element_buffer));
    
    return element_buffer;
}
torn_function void  
GFX_PushElementBuffer(GFX_ElementBuffer buffer)
{
    GFXD3D11_ElementBuffer* ehandle  = HANDLE_FetchElementBuffer(buffer.handle);
    GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(g_renderer->handle);
    
    ID3D11DeviceContext_IASetIndexBuffer(rhandle->device_context,
                                         ehandle->element_buffer,
                                         DXGI_FORMAT_R32_UINT,
                                         0);
    
    
}
torn_function void 
GFX_PopElementBuffer(GFX_ElementBuffer buffer)
{
    
    GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(g_renderer->handle);
    
    ID3D11DeviceContext_IASetIndexBuffer(rhandle->device_context,
                                         0,
                                         DXGI_FORMAT_R32_UINT,
                                         0);
    
    
}

//~ Shaders
//////////////////////////////////////////////////////


torn_function GFX_Shader 
GFX_CreateShader(u8* vertex_shader, u8* fragment_shader)
{
    
    GFX_Shader shader = {0};
    shader.vertex_shader = vertex_shader;
    shader.fragment_shader = fragment_shader;
    
    //~ jps: Creating the handle
    shader.handle =  HANDLE_CreateShader();
    GFXD3D11_Shader* shandle = HANDLE_FetchShader(shader.handle);
    GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(g_renderer->handle);
    
    //~ jps: Compile Shaders:
    i32 success = 1;
    i32 shader_length = 0;
    
    HRESULT hresult = 0; // We won't be using GFXD3D11_Validate because we would like some error output.
    ID3D10Blob* error;
    
    
    
    while (vertex_shader[shader_length++] != '\0');
    
    hresult = D3DCompile(vertex_shader, 
                         shader_length,
                         0,
                         0,
                         0,
                         "v_main",
                         "vs_4_0",
                         0,
                         0,
                         &shandle->bvs,
                         &error
                         );
    
    if (FAILED(hresult)) { success = 0; shader.vertex_error = ID3D10Blob_GetBufferPointer(error); };
    
    
    shader_length = 0;
    
    while (fragment_shader[shader_length++] != '\0');
    
    hresult = D3DCompile(fragment_shader, 
                         shader_length,
                         0,
                         0,
                         0,
                         "p_main",
                         "ps_4_0",
                         0,
                         0,
                         &shandle->bps,
                         &error
                         );
    
    if (FAILED(hresult)) { success = 0; shader.fragment_error = ID3D10Blob_GetBufferPointer(error); };
    
    if (success) 
    {
        
        GFXD3D11_Validate(ID3D11Device_CreateVertexShader(rhandle->device,
                                                          ID3D10Blob_GetBufferPointer(shandle->bvs),
                                                          ID3D10Blob_GetBufferSize(shandle->bvs),
                                                          0,
                                                          &shandle->vs));
        
        GFXD3D11_Validate(ID3D11Device_CreatePixelShader(rhandle->device,
                                                         ID3D10Blob_GetBufferPointer(shandle->bps),
                                                         ID3D10Blob_GetBufferSize(shandle->bps),
                                                         0,
                                                         &shandle->ps));
        return shader;
    }
    
    // If we failed to compile the shader.
    shader.program_error = "The vertex shader and/or the fragment shader did not compile!";
    return shader;
}

torn_function void
GFX_PushShader(GFX_Shader shader)
{
    //~ jps: Fetching the Handle
    GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(g_renderer->handle);
    GFXD3D11_Shader* shandle = HANDLE_FetchShader(shader.handle);
    //~ jps: Setting the VS & PS shader
    ID3D11DeviceContext_VSSetShader(rhandle->device_context,
                                    shandle->vs,
                                    0,
                                    0);
    ID3D11DeviceContext_PSSetShader(rhandle->device_context,
                                    shandle->ps,
                                    0,
                                    0);
}
torn_function void
GFX_PopShader(void)
{
    //~ jps: Fetching the Handle
    GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(g_renderer->handle);
    //~ jps: Setting the VS & PS shader
    ID3D11DeviceContext_VSSetShader(rhandle->device_context,
                                    0,
                                    0,
                                    0);
    ID3D11DeviceContext_PSSetShader(rhandle->device_context,
                                    0,
                                    0,
                                    0);
    
}
torn_function i32
GFX_ShaderFailed(GFX_Shader shader)
{
    return ((u8)shader.vertex_error | (u8)shader.fragment_error | (u8)shader.program_error);
}


torn_function void
GFX_SetTextureUnit(i32 unit)
{
    GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(g_renderer->handle);
    rhandle->texture_id = unit;
}

// [shader uniforms] TBAdded

torn_function void
GFX_HLSLShaderCBuffer(GFX_Shader* shader, i32 slot, i32 size, void* data)
{
    GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(g_renderer->handle);
    
    D3D11_BUFFER_DESC buffer_desc = {0};
    buffer_desc.ByteWidth = size;
    buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    
    D3D11_SUBRESOURCE_DATA subresource = {0};
    subresource.pSysMem = data;
    
    ID3D11Buffer* cbuffer;
    ID3D11Device_CreateBuffer(rhandle->device, &buffer_desc, &subresource, &cbuffer);
    
    GFX_PushShader(*shader);
    ID3D11DeviceContext_VSSetConstantBuffers(rhandle->device_context, slot, 1, &cbuffer);
    
    
}

//~ Vertex Array
//////////////////////////////////////////////////////

torn_function GFX_VertexArray
GFX_CreateVertexArray(void)
{
    GFX_VertexArray array = {0};
    
    //~ jps: creating handles
    array.handle = HANDLE_CreateVertexArray();
    
    return array;
    
}
torn_function void
GFX_SetVertexArrayFormat(GFX_Shader* shader, GFX_VertexArray* array, GFX_VertexArrayFlags flags)
{
    GFX_PushShader(*shader);
    array->flags = flags;
    
    GFXD3D11_VertexArray* vhandle = HANDLE_FetchVertexArray(array->handle);
    GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(g_renderer->handle);
    GFXD3D11_Shader* shandle = HANDLE_FetchShader(shader->handle);
    //~ jps: Input layouts!
    D3D11_INPUT_ELEMENT_DESC ied[4] = {0};
    
    i32 offset = 0;
    
    if (flags & GFX_VertexArray_Position) 
    {
        
        D3D11_INPUT_ELEMENT_DESC element = {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(GFX_Vertex, position), D3D11_INPUT_PER_VERTEX_DATA,0};
        ied[offset] = element;
        offset += 1;
    }
    
    if (flags & GFX_VertexArray_Color) 
    {
        
        D3D11_INPUT_ELEMENT_DESC element = {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(GFX_Vertex, color), D3D11_INPUT_PER_VERTEX_DATA,0};
        ied[offset] = element;
        offset += 1;
    }
    
    
    if (flags & GFX_VertexArray_Uv) 
    {
        D3D11_INPUT_ELEMENT_DESC element = {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(GFX_Vertex, uv), D3D11_INPUT_PER_VERTEX_DATA,0};
        ied[offset] = element;
        
        offset += 1;
    }
    
    if (flags & GFX_VertexArray_Normals) 
    {
        
        D3D11_INPUT_ELEMENT_DESC element = {"NORMALS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(GFX_Vertex, normals), D3D11_INPUT_PER_VERTEX_DATA,0};
        ied[offset] = element;
        offset += 1;
    }
    
    
    HRESULT result = ID3D11Device_CreateInputLayout(rhandle->device,
                                                    ied,
                                                    offset,
                                                    ID3D10Blob_GetBufferPointer(shandle->bvs),
                                                    ID3D10Blob_GetBufferSize(shandle->bvs),
                                                    &vhandle->input_layout);
    GFXD3D11_Validate(result);
    ID3D11DeviceContext_IASetInputLayout(rhandle->device_context, vhandle->input_layout);
    
}
torn_function void
GFX_PopVertexArray(void)
{
    GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(g_renderer->handle);
    ID3D11DeviceContext_IASetInputLayout(rhandle->device_context, 0);
}
torn_function void 
GFX_PushVertexArray(GFX_VertexArray* array)
{
    GFXD3D11_VertexArray* vhandle = HANDLE_FetchVertexArray(array->handle);
    if ((vhandle->input_layout != 0) && (vhandle != 0) ) /* Mimicking functionality.. */ 
    {
        GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(g_renderer->handle);
        ID3D11DeviceContext_IASetInputLayout(rhandle->device_context, vhandle->input_layout);
    }
}

//~ Texture
//////////////////////////////////////////////////////

torn_function GFX_Texture2D 
GFX_LoadDataTexture2D(u8* data, V2I size, GFX_ColorFormat format)
{
    GFX_Texture2D texture;
#if 0 
    texture.handle = HANDLE_CreateTexture2D();
    
    GFXD3D11_Texture2D* thandle = HANDLE_FetchTexture2D(texture.handle);
    GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(g_renderer->handle);
    
    D3D11_TEXTURE2D_DESC t2ddesc = {0};
    t2ddesc.MipLevels = 1;
    t2ddesc.ArraySize = 1;
    t2ddesc.Width = size.w;
    t2ddesc.Height = size.h;
    t2ddesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    t2ddesc.SampleDesc.Count = 1;
    t2ddesc.Usage = D3D11_USAGE_IMMUTABLE;
    t2ddesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    
    
    D3D11_SUBRESOURCE_DATA subresource = {0};
    subresource.pSysMem = data;
    subresource.SysMemPitch = size.w * sizeof(u32);
    
    
    ID3D11Texture2D* d3d11texture;
    GFXD3D11_Validate(ID3D11Device_CreateTexture2D(rhandle->device,
                                                   &t2ddesc,
                                                   &subresource,
                                                   &d3d11texture));
    GFXD3D11_Validate(ID3D11Device_CreateShaderResourceView(rhandle->device,
                                                            (ID3D11Resource*)d3d11texture,
                                                            0,
                                                            &thandle->srv));
    ID3D11Texture2D_Release(d3d11texture);
    
#endif
    return texture;
    
}

internal i32 
HELPER_Compare(const u8* compare , const u8* source)
{
    i32 success = 1;
    while (*compare != '\0')
    {
        if (*compare != *source)
            return 0;
        success &= (*compare && *source);
        *compare++;
        *source++;
    }
    return success;
    
}

torn_function GFX_Texture2D
GFX_LoadTexture2D( u8* path)
{
    GFX_Texture2D texture = {0};
#if 0 
    texture.path = path;
    TORN_Log("GFX: D3D11: Attempting to load \"%s\"\n", path);
    u8* data = stbi_load(path, &texture.size.w, &texture.size.h, &texture.channels, 0);
    
    if (data == 0)
    {
        TORN_Log("GFX: D3D11: \"%s\" \was invalid! Can't load image, returing texture as it is.\n", path);
        
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
    
    if (HELPER_Compare(ext, "jpg") || HELPER_Compare(ext, "jpeg")) { format = GFX_Format_RGB; }
    if (HELPER_Compare(ext, "png")) { format = GFX_Format_RGBA; }
    if (HELPER_Compare(ext, "bmp")) { format = GFX_Format_BGR; }
    texture = GFX_LoadDataTexture2D(data, texture.size, format);
    GFX_PushTexture(&g_renderer->default_texture);
#endif
    return texture;
}

//~ Renderer
//////////////////////////////////////////////////////


torn_function GFX_Renderer* 
GFX_CreateRenderer(OS_App* app)
{
    //~ jps: Creating the renderer & handle. 
    
    GFX_Renderer* renderer = MEM_ArenaAlloc(MEM_GetDefaultArena(), 
                                            sizeof(GFX_Renderer));
    
    renderer->handle = HANDLE_CreateRenderer();
    GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(renderer->handle);
    OS_Win32Handle* whandle = (OS_Win32Handle*)OS_GetHandle(app);
    
    //~ jps: Configurating the renderer!
    renderer->window_size = &app->size;
    
    //~ jps: Enabling the use of g_renderer 
    GFX_PushRenderer(renderer);
    
    //~ jps: Initializing D3D11
    
    DXGI_SWAP_CHAIN_DESC swapchain_desc = {0}; 
    swapchain_desc.OutputWindow             = whandle->window;
    swapchain_desc.BufferDesc.Format        = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchain_desc.BufferUsage              = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchain_desc.SampleDesc.Count         = 1;
    swapchain_desc.Windowed                 = 1;
    swapchain_desc.BufferCount              = 2;
    
    GFXD3D11_Validate(D3D11CreateDeviceAndSwapChain(0,
                                                    D3D_DRIVER_TYPE_HARDWARE,
                                                    0,
                                                    0,
                                                    0,
                                                    0, 
                                                    D3D11_SDK_VERSION,
                                                    &swapchain_desc,
                                                    &rhandle->swapchain,
                                                    &rhandle->device,
                                                    0,
                                                    &rhandle->device_context));
    
    
    //~ jps: Shaders
    u8* vertex_shader =
        "struct vertex_output {\n"
        "\tfloat4 position : SV_POSITION;\n"
        "\tfloat4 color    : COLOR;\n"
        "\tfloat2 uv       : TEXCOORD;\n"
        "\tfloat4 normals  : NORMALS;\n"
        "};\n"
        
        "cbuffer cbuffer0 : register(b0) \n{"
        "\t float4x4 orthographic;\n"
        "\t float4x4 model;\n"
        "\t float4x4 view;\n"
        "};\n"
        
        "vertex_output v_main(float4 position : POSITION, float4 color : COLOR, float2 uv : TEXCOORD, float4 normals : NORMALS) { \n"
        "\tvertex_output output;\n"
        "\tfloat4 mvp_position = mul(model, mul(view, mul(orthographic, position)));\n"
        "\toutput.position = mvp_position;\n"
        "\toutput.color    = color;\n"
        "\toutput.uv = uv;\n"
        "\toutput.normals = normals;\n"
        "\treturn output;\n"
        "}\n\0";
    
    u8* fragment_shader = 
        "sampler tsample : register(s0);\n"
        "Texture2D<float4> ttexture : register(t0);\n"
        
        "float4 p_main(float4 position : SV_POSITION, float4 color : COLOR, float2 uv : TEXCOORD, float4 normals : NORMALS) : SV_TARGET { \n"
        "float4 tex = ttexture.Sample(tsample, uv);\n"
        "return color;\n"
        "}\n\0";
    
    renderer->default_shader = GFX_CreateShader(vertex_shader, fragment_shader);
    // jps: shader error checking
    if (renderer->default_shader.vertex_error)
    {
        TORN_Error(renderer->default_shader.vertex_error, "Vertex Shader: Compilation Error (internal-default-shader)");
    }
    if (renderer->default_shader.fragment_error)
    {
        TORN_Error(renderer->default_shader.fragment_error, "Fragment Shader: Compilation Error (internal-default-shader)");
    }
    if (renderer->default_shader.program_error)
    {
        TORN_Error(renderer->default_shader.program_error, "Program: Error (internal-default-shader)");
    }
    GFX_PushShader(renderer->default_shader);
    
    //~ vertex array
    renderer->vertex_array = GFX_CreateVertexArray();
    
    //~ vertex buffer
    renderer->vertex_buffer = GFX_CreateVertexBuffer(MAX_BATCHABLE_SIZE);
    GFX_PushVertexBuffer(&renderer->vertex_buffer);
    
    
    //~ format
    GFX_SetVertexArrayFormat(&renderer->default_shader, &renderer->vertex_array, 
                             GFX_VertexArray_Position |
                             GFX_VertexArray_Color    |
                             GFX_VertexArray_Uv       |
                             GFX_VertexArray_Normals);
    
    GFX_PushVertexArray(&renderer->vertex_array);
    
    GFX_LoadMVP(&renderer->default_shader, renderer);
    
    D3D11_SAMPLER_DESC sampler_desc = {0};
    sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    GFXD3D11_Validate(ID3D11Device_CreateSamplerState(rhandle->device,
                                                      &sampler_desc,
                                                      &rhandle->sampler));
    
    
    ID3D11DeviceContext_PSSetSamplers(rhandle->device_context,
                                      rhandle->sample_id,
                                      1,
                                      &rhandle->sampler);
    
    
    D3D11_BLEND_DESC blend_desc  ={0};
    blend_desc.RenderTarget[0].BlendEnable = 1;
    blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA,
    blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA,
    blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD,
    blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA,
    blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA,
    blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD,
    blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
    ID3D11Device_CreateBlendState(rhandle->device, 
                                  &blend_desc,
                                  &rhandle->blend);
    ID3D11DeviceContext_OMSetBlendState(rhandle->device_context,
                                        rhandle->blend,
                                        0,
                                        ~0U);
    
    
    
    D3D11_RASTERIZER_DESC rasterizer = {0};
    rasterizer.FillMode = D3D11_FILL_SOLID;
    rasterizer.CullMode = D3D11_CULL_NONE;
    ID3D11Device_CreateRasterizerState(rhandle->device, &rasterizer, &rhandle->rasterizer);
    ID3D11DeviceContext_RSSetState(rhandle->device_context, rhandle->rasterizer);
    
    GFX_Resize(v2i(0,0), app->size);
    
    
    D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {0};
    depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
    depth_stencil_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    depth_stencil_desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    ID3D11Device_CreateDepthStencilState(rhandle->device,
                                         &depth_stencil_desc,
                                         &rhandle->ds);
    ID3D11DeviceContext_OMSetDepthStencilState(rhandle->device_context,
                                               rhandle->ds,
                                               0);
    
    
    
    ID3D11Texture2D* buffer;
    GFXD3D11_Validate(IDXGISwapChain_GetBuffer(rhandle->swapchain,
                                               0,
                                               &IID_ID3D11Texture2D,
                                               (LPVOID*)&buffer));
    
    GFXD3D11_Validate(ID3D11Device_CreateRenderTargetView(rhandle->device,
                                                          (ID3D11Resource*)buffer,
                                                          0,
                                                          &rhandle->view));
    GFXD3D11_Validate(ID3D11Texture2D_Release(buffer));
    
    
    
    D3D11_TEXTURE2D_DESC depth_desc = {0};
    depth_desc.Width = app->size.w;
    depth_desc.Height = app->size.h;
    depth_desc.MipLevels = 1;
    depth_desc.ArraySize = 1;
    depth_desc.Format = DXGI_FORMAT_D32_FLOAT;
    depth_desc.SampleDesc.Count = 1;
    depth_desc.Usage = D3D11_USAGE_DEFAULT;
    depth_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    
    
    ID3D11Texture2D* depth_texture;
    ID3D11Device_CreateTexture2D(rhandle->device,
                                 &depth_desc,
                                 0,
                                 &depth_texture);
    
    ID3D11Device_CreateDepthStencilView(rhandle->device,
                                        (ID3D11Resource*)depth_texture,
                                        0,
                                        &rhandle->vds);
    ID3D11Texture2D_Release(depth_texture);
    
    
    ID3D11DeviceContext_OMSetRenderTargets(rhandle->device_context,
                                           1, &rhandle->view, rhandle->vds);
    
    //~ jps: We obtain the device information.
    IDXGIDevice* dxgi_device;
    ID3D11Device_QueryInterface(rhandle->device, &IID_IDXGIDevice, (void**)&dxgi_device);
    IDXGIDevice1_GetAdapter(dxgi_device, &rhandle->adapter);
    IDXGIAdapter_GetDesc(rhandle->adapter, &rhandle->adapter_desc);
    
    
    
    TORN_Log("GFX: D3D11: Loaded D3D11 Renderer\n");
    TORN_Log("GFX: D3D11: Version: %s\n", GFX_GetBackendVersion());
    TORN_Log("GFX: D3D11: Vendor: %s\n", GFX_GetGPUVendor());
    TORN_Log("GFX: D3D11: GPU name: %s\n", GFX_GetGPUName());
    
    
    return (renderer);
    
}
torn_function void
GFX_Clear(GFX_Renderer* renderer)
{
    //~ jps: Getting the handle from the g_renderer.
    GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(renderer->handle);
    
    //~ jps: CLeraing the renderer!
    r32 clear_color[4] = 
    {
        renderer->clear_color.r / 255.0f,
        renderer->clear_color.g / 255.0f,
        renderer->clear_color.b / 255.0f,
        renderer->clear_color.a / 255.0f
    };
    
    ID3D11DeviceContext_ClearRenderTargetView(rhandle->device_context,
                                              rhandle->view,
                                              clear_color);
    
    
    
}
torn_function void
GFX_Present(GFX_Renderer* renderer)
{
    
    //~ jps: Getting the handle from the g_renderer.
    GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(renderer->handle);
    
    //~ jps: CLeraing the renderer!
    HRESULT status = IDXGISwapChain_Present(rhandle->swapchain, renderer->vsync ? 1 : 0,0);
    
    // We are unable to do vsync when the window is occluded.
    if (status == DXGI_STATUS_OCCLUDED && renderer->vsync)
        Sleep(10);
    
}
torn_function void
GFX_Resize(V2I pos, V2I size) 
{
    if (g_renderer != 0)
    {
        //~ jps: Getting the handle from the g_renderer.
        GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(g_renderer->handle);
        
        //~ jps: Setting the viewport!
        D3D11_VIEWPORT viewport = {0};
        viewport.Width  = size.w;
        viewport.MaxDepth  = 1;
        viewport.Height = size.h;
        viewport.TopLeftX = pos.x;
        viewport.TopLeftY = pos.y;
        
        ID3D11DeviceContext_RSSetViewports(rhandle->device_context,
                                           1, 
                                           &viewport);
        ID3D11DeviceContext_RSSetState(rhandle->device_context, rhandle->rasterizer);
        
        GFX_LoadMVP(&g_renderer->default_shader, g_renderer);
    }
    
}


torn_function void
GFX_SetClearColor(V4F color)
{
    g_renderer->clear_color = color;
}

///// for internal use only, to be sent to cbuffer in shader. (b0)
typedef struct GFX_CBuffer GFX_CBuffer;
struct GFX_CBuffer
{
    M_Mat4x4 orthographic;
    M_Mat4x4 view;
    M_Mat4x4 model;
};

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
    GFX_CBuffer cbuffer = {0};
    cbuffer.orthographic    =   orthographic_view;
    cbuffer.view            =   view;
    cbuffer.model           =   model;
    
    GFX_HLSLShaderCBuffer(shader, 0,sizeof(cbuffer), &cbuffer);
    
    
}



//~ IM-Based Rendering
//////////////////////////////////////////////////////


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
GFX_PushTexture(GFX_Texture2D* texture)
{
#if 0 
    g_texture2d = texture;
    GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(g_renderer->handle);
    GFXD3D11_Texture2D* thandle = HANDLE_FetchTexture2D(texture->handle);
    
    ID3D11DeviceContext_PSSetSamplers(rhandle->device_context,
                                      rhandle->sample_id,
                                      1,
                                      &rhandle->sampler);
    
    ID3D11DeviceContext_PSSetShaderResources(rhandle->device_context,
                                             rhandle->texture_id,
                                             1,
                                             &thandle->srv);
    
#endif
}

torn_function void
GFX_PopTexture(void)
{
#if 0
    GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(g_renderer->handle);
    
    
    ID3D11DeviceContext_PSSetSamplers(rhandle->device_context,
                                      rhandle->sample_id,
                                      1,
                                      0);
    
    ID3D11DeviceContext_PSSetShaderResources(rhandle->device_context,
                                             rhandle->texture_id,
                                             1,
                                             0);
#endif
}
torn_function void
GFX_PushV3(V3F v_pos)
{
    GFX_Vertex vertex_data = {0};
    vertex_data.position = v_pos;
    vertex_data.color =    g_renderer->color;
    vertex_data.uv =       g_renderer->uv;
    vertex_data.normals=   g_renderer->normals;
    
    
    GFX_AddVertexBuffer(g_renderer->offset, sizeof(vertex_data),  vertex_data);
    g_renderer->offset += sizeof(GFX_Vertex);
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
    GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(g_renderer->handle);
    GFXD3D11_VertexBuffer* vbhandle = HANDLE_FetchVertexBuffer(g_renderer->vertex_buffer.handle);
    
    u32 stride = sizeof(GFX_Vertex);
    u32 offset = 0;
    
    ID3D11DeviceContext_IASetVertexBuffers(rhandle->device_context,
                                           0,
                                           1,
                                           &vbhandle->vertex_buffer,
                                           &stride,
                                           &offset);
    
    switch (primitive)
    {
        case GFX_Triangles: 
        {
            ID3D11DeviceContext_IASetPrimitiveTopology(rhandle->device_context,
                                                       D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        } break;
        case GFX_TriangleStrip: 
        {
            ID3D11DeviceContext_IASetPrimitiveTopology(rhandle->device_context,
                                                       D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        } break;
        default: 
        {
            ID3D11DeviceContext_IASetPrimitiveTopology(rhandle->device_context,
                                                       D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        } break;
    }
    ID3D11DeviceContext_Draw(rhandle->device_context, 
                             g_renderer->offset, 0);
    
    g_renderer->offset = 0;
}
torn_function void
GFX_EndDrawIndexed(i32 indices, GFX_Primitive primitive)
{
    GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(g_renderer->handle);
    GFXD3D11_VertexBuffer* vbhandle = HANDLE_FetchVertexBuffer(g_renderer->vertex_buffer.handle);
    
    u32 stride = sizeof(GFX_Vertex);
    u32 offset = 0;
    
    ID3D11DeviceContext_IASetVertexBuffers(rhandle->device_context,
                                           0,
                                           1,
                                           &vbhandle->vertex_buffer,
                                           &stride,
                                           &offset);
    
    switch (primitive)
    {
        case GFX_Triangles: 
        {
            ID3D11DeviceContext_IASetPrimitiveTopology(rhandle->device_context,
                                                       D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        } break;
        case GFX_TriangleStrip: 
        {
            ID3D11DeviceContext_IASetPrimitiveTopology(rhandle->device_context,
                                                       D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        } break;
        default: 
        {
            ID3D11DeviceContext_IASetPrimitiveTopology(rhandle->device_context,
                                                       D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        } break;
        
    }
    
    
    ID3D11DeviceContext_DrawIndexed(rhandle->device_context,
                                    indices,0,0);
    
    
    g_renderer->offset = 0;
}


//~ Framebuffers
//////////////////////////////////////////////////////

torn_function GFX_Framebuffer GFX_CreateFramebuffer(GFX_Renderer* renderer, GFX_ColorFormat color_format);
torn_function void GFX_PushFramebuffer(GFX_Framebuffer framebuffer);
torn_function void GFX_PopFramebuffer(void);

//~ Misc
//////////////////////////////////////////////////////

torn_function const i8*
GFX_GetBackendVersion(void)
{
    return "11";
}
torn_function const i8*
GFX_GetBackend(void)
{
    return "DirectX";
}

torn_function const i8* 
GFX_GetGPUVendor(void)
{
    
    GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(g_renderer->handle);
    i32 vendor_id = rhandle->adapter_desc.VendorId;
    
    
    switch (vendor_id)
    {
        case (PCIID_NVIDIA):
        {
            return "NVIDIA";
        } break;
        
        case (PCIID_AMD):
        {
            return "AMD";
        } break;
        case (PCIID_INTEL):
        {
            return "Intel";
        } break;
        case (PCIID_MICROSOFT_BASIC_RENDERER):
        {
            return "Microsoft Basic Renderer";
        } break;
        
        default:
        {
            return "Unknown";
        } break;
    }
    
    
}

torn_function const i8*
GFX_GetGPUName(void)
{
    GFXD3D11_Renderer* rhandle = HANDLE_FetchRenderer(g_renderer->handle);
    
    // 256 because adapter_desc.Descriptor is 256 bytes.
    u8* output = MEM_ArenaAlloc(MEM_GetDefaultArena(), 256);
    WideCharToMultiByte(CP_ACP,
                        WC_COMPOSITECHECK,
                        rhandle->adapter_desc.Description,
                        sizeof(rhandle->adapter_desc.Description),
                        output,
                        256,
                        0,
                        0);
    return output;
}


