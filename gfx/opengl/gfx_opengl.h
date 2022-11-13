#ifndef GFX_OPENGL_H
#define GFX_OPENGL_H

#include <os/os.h>
#include <base/base.h>


//~ gfx_opengl only has generic loader functions
// to load opengl.
torn_function i32 GFXGL_LoadOpenGL(OS_App* app);
torn_function void GFXGL_Present(void);

#endif