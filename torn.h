#ifndef TORN_H
#define TORN_H
#ifdef __cplusplus
extern "C" {
#endif
#include <base/base.h>
#include <os/os_memory.h>
#include <memory/arena.h>
#include <os/os.h>
#ifdef TORN_OPENGL
#include <third-party/glad/glad.h>
#endif
#include <gfx/gfx.h>
#include <gfx/gfx_2d.h>
#include <utils/utils.h>
#include <sound/sound.h>
#include <ui/widgets.h>
    
    torn_function inline void
        TORN_Init(void)
    {
#ifdef TORN_VERBOSE
        char* symbol = 
            "88888 .d88b. 888b. 8b  8 \n"
            "  8   8P  Y8 8  .8 8Ybm8 \n"
            "  8   8b  d8 8wwK' 8  \"8 \n"
            "  8   `Y88P' 8  Yb 8   8 \n"
            "TORN - The Graphics Framework.\n"
            "=--------------------------=\n";
        
        
        TORN_Log("%s\n", symbol);
#endif
        TORN_Log("TORN: You've enabled verbose logging!\n");
        TORN_Log("TORN: To disable it, please undefine TORN_VERBOSE!\n");
        TORN_Log("TORN: Initialized TORN %s\n", TORN_VERSION);
        // if exit != 1, then everything succeeded.
        // if exit == 1, then it failed.
        
        
        MEM_Init();
        
    }
    
    
    torn_function inline void
        TORN_Destroy(void)
    {
        TORN_Log("TORN: Destroying all modules for TORN\n");
        MEM_Destroy();
        TORN_Log("TORN: Destroyed all modules\n");
    }
#ifdef __cplusplus
}
#endif.

#endif


