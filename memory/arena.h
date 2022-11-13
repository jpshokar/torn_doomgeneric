#ifndef MEMORY_ARENA_H
#define MEMORY_ARENA_H


#include <base/base.h>
typedef struct MEM_Arena MEM_Arena;
struct MEM_Arena
{
    u8* address_space;
    i32 end;
    i32 size;
};

torn_function MEM_Arena MEM_ArenaInit(i32 size);
torn_function void* MEM_ArenaAlloc(MEM_Arena* arena, i32 size);
torn_function void MEM_ArenaReAlloc(MEM_Arena* arena, i32 size);
torn_function void MEM_ArenaFree(MEM_Arena* arena);


torn_function i32 MEM_Init(void);
torn_function void MEM_Destroy(void);
torn_function MEM_Arena* MEM_GetDefaultArena(void);




#endif 
