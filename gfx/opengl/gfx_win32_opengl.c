#include <windows.h>
#include "gfx_opengl.h"

#ifdef TORN_OPENGL
#include <third-party/glad/glad.h>
#endif

typedef struct GFXGL_Win32Handle GFXGL_Win32Handle;
struct GFXGL_Win32Handle
{
    HDC   device_context;
    HGLRC gl_rendering_context;
};


global GFXGL_Win32Handle gl_handle;

torn_function i32
GFXGL_LoadOpenGL(OS_App* app)
{
    PIXELFORMATDESCRIPTOR pixelformat = { 
        .nSize = sizeof(PIXELFORMATDESCRIPTOR), 
        .nVersion = 1,
        .dwFlags = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,    
        .iPixelType = PFD_TYPE_RGBA,        
        .cColorBits = 24
    }; 
    
    OS_Win32Handle* handle = (OS_Win32Handle*)OS_GetHandle(app);
    
    gl_handle.device_context = GetDC(handle->window);
    i32 suitable_pixel_format = ChoosePixelFormat(gl_handle.device_context, &pixelformat);
    SetPixelFormat(gl_handle.device_context, suitable_pixel_format, &pixelformat );
    
    gl_handle.gl_rendering_context = wglCreateContext(gl_handle.device_context);
    wglMakeCurrent(gl_handle.device_context, gl_handle.gl_rendering_context);
    if (!gladLoadGL())
        return 1;
    
    
    
    return 0;
    
}

torn_function void
GFXGL_Present(void)
{
    SwapBuffers(gl_handle.device_context);
}