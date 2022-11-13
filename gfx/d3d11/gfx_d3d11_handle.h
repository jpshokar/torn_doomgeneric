

#ifndef GFX_D3D11_HANDLE_H
#define GFX_D3D11_HANDLE_H

#include <os/os.h>

#define HANDLE_Fetch(st, ptr) (st*)ptr
#define HANDLE_Create(st) MEM_ArenaAlloc(MEM_GetDefaultArena(), sizeof(st))


// Check if a DirectX 11 function has failed
#define GFXD3D11_Validate(hresult) if (FAILED(hresult)) __debugbreak()




/*
* Handle Nomenclature  
* -------------------------------------------------------------------------
*  Handle typically have the prefix being the first letter of the handles'  
*  parent, and then having handle at the end.
*
*  ie.
*  GFXD3D11_Renderer handle would be associated as rhandle.
*
*  Sometimes, the second letter or, first letter of the second letter may be used instead to make the handle not ambigous.
*/

typedef struct GFXD3D11_Renderer GFXD3D11_Renderer;
struct GFXD3D11_Renderer
{
    ID3D11Device* device;
    ID3D11DeviceContext* device_context;
    IDXGISwapChain* swapchain;
    ID3D11RenderTargetView* view;
    IDXGIAdapter* adapter;
    DXGI_ADAPTER_DESC adapter_desc;
    
    
    ID3D11SamplerState* sampler;
    ID3D11BlendState* blend;
    ID3D11RasterizerState* rasterizer;
    ID3D11DepthStencilState* ds;
    ID3D11DepthStencilView* vds;
    i32 texture_id;
    i32 sample_id;
    i32 end;
    
    
};

#define HANDLE_CreateRenderer() HANDLE_Create(GFXD3D11_Renderer)
#define HANDLE_FetchRenderer(ptr) HANDLE_Fetch(GFXD3D11_Renderer, ptr)

typedef struct GFXD3D11_Shader GFXD3D11_Shader;
struct GFXD3D11_Shader
{
    ID3DBlob* bvs;
    ID3DBlob* bps;
    
    ID3D11VertexShader* vs;
    ID3D11PixelShader* ps;
    
    
};

#define HANDLE_CreateShader() HANDLE_Create(GFXD3D11_Shader)
#define HANDLE_FetchShader(ptr) HANDLE_Fetch(GFXD3D11_Shader, ptr)


typedef struct GFXD3D11_VertexArray GFXD3D11_VertexArray;
struct GFXD3D11_VertexArray
{
    ID3D11InputLayout* input_layout;
};
#define HANDLE_CreateVertexArray() HANDLE_Create(GFXD3D11_VertexArray)
#define HANDLE_FetchVertexArray(ptr) HANDLE_Fetch(GFXD3D11_VertexArray, ptr)


typedef struct GFXD3D11_VertexBuffer GFXD3D11_VertexBuffer;
struct GFXD3D11_VertexBuffer
{
    ID3D11Buffer* vertex_buffer;
    i32 vertices;
    D3D11_MAPPED_SUBRESOURCE subresource;
};
#define HANDLE_CreateVertexBuffer() HANDLE_Create(GFXD3D11_VertexBuffer)
#define HANDLE_FetchVertexBuffer(ptr) HANDLE_Fetch(GFXD3D11_VertexBuffer, ptr)


typedef struct GFXD3D11_ElementBuffer GFXD3D11_ElementBuffer;
struct GFXD3D11_ElementBuffer
{
    ID3D11Buffer* element_buffer;
    
};
#define HANDLE_CreateElementBuffer() HANDLE_Create(GFXD3D11_ElementBuffer)
#define HANDLE_FetchElementBuffer(ptr) HANDLE_Fetch(GFXD3D11_ElementBuffer, ptr)



typedef struct GFXD3D11_Texture2D GFXD3D11_Texture2D;
struct GFXD3D11_Texture2D
{
    ID3D11ShaderResourceView* srv;
    
};
#define HANDLE_CreateTexture2D() HANDLE_Create(GFXD3D11_Texture2D)
#define HANDLE_FetchTexture2D(ptr) HANDLE_Fetch(GFXD3D11_Texture2D, ptr)




/* PCI IDs for getting the vendor name - only getting the major vendors */

#define PCIID_NVIDIA 0x10DE
#define PCIID_AMD    0x1002
#define PCIID_INTEL  0x8086
// Sometimes, you may require software rendering as a fallback for any hardware accelerated programs.
#define PCIID_MICROSOFT_BASIC_RENDERER 0x1414


#endif
