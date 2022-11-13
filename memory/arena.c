#include "arena.h"
#include <os/os_memory.h>
#include <stdint.h> // only for uintptr_t!


torn_function MEM_Arena
MEM_ArenaInit(i32 size)
{
    MEM_Arena arena = {0};
    arena.size = size;
    arena.address_space = OS_Allocate(size);
    return arena;
}

torn_function void*
MEM_ArenaAlloc(MEM_Arena* arena, i32 size)
{
    
    if (arena->end + size <= arena->size){
        
        i32 end = arena->end;
        // for faster memory access; check if (end) is a power of 2, and if not allign to power of two: 
        if (end & (end-1) != 0)
        {
            uintptr_t mod = (uintptr_t)(&arena->address_space[end]) & ((sizeof(void*)*2)-1);
            if (mod != 0)
            {
                end += (sizeof(void*)*2) - mod;
            }
        }
        
        void* loc = &arena->address_space[end];
        
        arena->end += size;
        TORN_Log("MEMORY: Allocated %i bytes at location %p\nMEMORY: overall arena usage before resize: (%i\\%i)\n", size, loc,
                 arena->end,
                 arena->size);
        return loc;
    } else 
    {
        TORN_Log("MEMORY: Attempt to allocate %i bytes was refused.\n", size);
        TORN_Log("MEMORY: Out of memory (%i bytes used) Resizing to accomodate this change!\n", arena->size);
        ///~ out of memory, so we the arena  prev_size( prev_size + size ) * 2. 
        ///~ note: The pointed address will be the same, even when resized.
        MEM_ArenaReAlloc(arena, arena->size * 2);
        return MEM_ArenaAlloc(arena, size);
    }
    
}

torn_function void
MEM_ArenaReAlloc(MEM_Arena* arena, i32 size)
{
    
    i32 old_size = arena->size;
    arena->size = size;
    
    u8* new_address_space = OS_Allocate(arena->size);
    
    for(i32 i = 0;i<arena->end;i++)
    {
        new_address_space[i] = arena->address_space[i];
    }
    OS_Free(arena->address_space, old_size);
    
    arena->address_space = new_address_space;
    TORN_Log("MEMORY: Resized to %i bytes\n", arena->size);
    
}
torn_function void 
MEM_ArenaFree(MEM_Arena* arena)
{
    arena->end = 0;
    OS_Free(arena->address_space, arena->size);
    TORN_Log("MEMORY: Arena was freed.\n");
}

global MEM_Arena arena;


torn_function i32
MEM_Init(void)
{
    arena = MEM_ArenaInit(5000000); // 5MiB of VIRTUAL MEMORY!
    TORN_Log("MEMORY: Arena was initialized.\n");
    if (arena.address_space == 0)
        return 1;
    return 0;
}
torn_function MEM_Arena*
MEM_GetDefaultArena(void)
{
    return &arena;
}
torn_function void
MEM_Destroy(void)
{
    MEM_ArenaFree(&arena);
}
