

#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#include <memory/arena.h>
#include <base/base.h>

typedef struct RS_Resource RS_Resource;

struct RS_Resource
{
    u8*   key;
    void* value;
    
    i32 references;
};


typedef struct RS_ResourceManager RS_ResourceManager;
struct RS_ResourceManager
{
    RS_Resource* resources;
    i32 size;
    i32 used;
    MEM_Arena arena;
};


torn_function RS_ResourceManager RS_CreateResourceManager();
torn_function void RS_AddToResourceManager(RS_ResourceManager* resource_manager,
                                           void* value, 
                                           i32 size,
                                           u8* key);

torn_function void RS_RemoveFromResourceManager(RS_ResourceManager* resource_manager,
                                                u8* key);

torn_function void* RS_GetElement(RS_ResourceManager* resource_manager, u8* key);





#endif 
