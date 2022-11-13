#include "resource_manager.h"
#include <utils/utils.h>

// probably bug-prone: fix-me!
torn_function RS_ResourceManager RS_CreateResourceManager()
{
    RS_ResourceManager resource_manager = {0};
    resource_manager.arena = MEM_ArenaInit(50000);
    
    resource_manager.resources = MEM_ArenaAlloc(&resource_manager.arena, sizeof(RS_Resource) * 1024);
    
    resource_manager.size = sizeof(RS_Resource) * 1024;
    return resource_manager;
    
}
torn_function void RS_AddToResourceManager(RS_ResourceManager* resource_manager,
                                           void* value,
                                           i32 size,
                                           u8* key)
{
    if (resource_manager->used + size >= resource_manager->size)
    {
        resource_manager->size *= 2;
        MEM_ArenaReAlloc(&resource_manager->arena, resource_manager->size);
        RS_AddToResourceManager(resource_manager, value, size, key);
        return;
    } else 
    {
        resource_manager->resources[resource_manager->used].key = key;
        resource_manager->resources[resource_manager->used].value = value;
        resource_manager->used += 1;
    }
    
}


torn_function void RS_RemoveFromResourceManager(RS_ResourceManager* resource_manager,
                                                u8* key)
{
    for (i32 i = 0; i<resource_manager->used;i++)
    {
        if (U_Compare(resource_manager->resources[i].key, key)) {
            
            // i - to be removed
            
            for (i32 j = i - 1; j < resource_manager->used - 1; j++)
            {
                resource_manager->resources[j] = resource_manager->resources[j+1];
            }
            resource_manager->used -= 1;
            
            
        }
    }
}


torn_function void* RS_GetElement(RS_ResourceManager* resource_manager, u8* key)
{
    for (i32 i = 0; i<resource_manager->used;i++)
    {
        if (U_Compare(resource_manager->resources[i].key, key)) { return resource_manager->resources[i].value; }
    }
    TORN_Error(key, "Resource Manager: Key Not Found");
    return 0;
    
}
