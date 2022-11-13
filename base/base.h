#ifndef BASE_H
#define BASE_H
#include "settings.h"
typedef long  i64;
typedef int   i32;
typedef short i16;
typedef char  i8;
typedef unsigned long  u64;
typedef unsigned int   u32;
typedef unsigned short u16;
typedef unsigned char  u8;
typedef float r32;
typedef i32 b32;

#define internal static
#define local static
#define global static

#ifdef TORN_EXPORT
#define torn_function __declspec(dllexport)
#elif TORN_IMPORT
#define torn_function  __declspec(dllimport)
#else
#define torn_function
#endif

#ifdef _WIN32
#define TORN_DEFAULT CW_USEDEFAULT
#else
#define TORN_DEFAULT 0
#endif

typedef union V4I V4I;
typedef union V3I V3I;
typedef union V2I V2I;

typedef union V4F V4F;
typedef union V3F V3F;
typedef union V2F V2F;


union V4F
{
    struct 
    {
        r32 r;
        r32 g;
        r32 b;
        r32 a;
    };
    struct
    {
        r32 x;
        r32 y;
        r32 z;
        r32 w;
    };
};

union V3F
{
    struct
    {
        r32 x;
        r32 y;
        r32 z;
    };
    
    struct
    {
        r32 r;
        r32 g;
        r32 b;
    };
};

union V2F
{
    struct 
    {
        r32 x;
        r32 y;
    };
    struct
    {
        r32 w;
        r32 h;
    };
};


torn_function V4F V4F_Construct(r32 r, r32 g, r32 b, r32 a);
torn_function V3F V3F_Construct(r32 x, r32 y, r32 z);
torn_function V2F V2F_Construct(r32 x, r32 y);

#define v4(...) V4F_Construct(__VA_ARGS__)
#define v3(...) V3F_Construct(__VA_ARGS__)
#define v2(...) V2F_Construct(__VA_ARGS__)




union V4I
{
    struct 
    {
        i32 r;
        i32 g;
        i32 b;
        i32 a;
    };
    struct
    {
        i32 x;
        i32 y;
        i32 z;
        i32 w;
    };
};

union V3I
{
    struct
    {
        i32 x;
        i32 y;
        i32 z;
    };
    
    struct
    {
        i32 r;
        i32 g;
        i32 b;
    };
};

union V2I
{
    struct 
    {
        i32 x;
        i32 y;
    };
    struct
    {
        i32 w;
        i32 h;
    };
};

torn_function V4I V4I_Construct(i32 r, i32 g, i32 b, i32 a);
torn_function V3I V3I_Construct(i32 x, i32 y, i32 z);
torn_function V2I V2I_Construct(i32 x, i32 y);
#define v4i(...) V4I_Construct(__VA_ARGS__)
#define v3i(...) V3I_Construct(__VA_ARGS__)
#define v2i(...) V2I_Construct(__VA_ARGS__)
// Versioning scheme:
// VMajor.VMinor
#define TORN_VERSION "1.0"



#endif 
