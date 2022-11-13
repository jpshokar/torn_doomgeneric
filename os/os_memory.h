#ifndef OS_MEMORY_H
#define OS_MEMORY_H

#include <base/base.h>

torn_function void* OS_Allocate(i32 size);
torn_function void  OS_Free(void* address, u32 size);


#endif
