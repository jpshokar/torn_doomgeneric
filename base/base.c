#include "base.h"
torn_function V4F 
V4F_Construct(r32 r, r32 g, r32 b, r32 a) 
{
    V4F v4f; 
    v4f.r = r;
    v4f.g = g;
    v4f.b = b;
    v4f.a = a;
    return v4f;
};
torn_function V3F
V3F_Construct(r32 x, r32 y, r32 z)
{
    V3F v3f;
    v3f.x = x;
    v3f.y = y;
    v3f.z = z;
    return v3f;
}
torn_function V2F
V2F_Construct(r32 x, r32 y)
{
    V2F v2f;

    v2f.x = x;
    v2f.y = y;
    return v2f;
}

torn_function V4I
V4I_Construct(i32 r, i32 g, i32 b, i32 a) 
{
    V4I v4f; 
    v4f.r = r;
    v4f.g = g;
    v4f.b = b;
    v4f.a = a;
    return v4f;
};
torn_function V3I
V3I_Construct(i32 x, i32 y, i32 z)
{
    V3I v3f;
    v3f.x = x;
    v3f.y = y;
    v3f.z = z;
    return v3f;
}
torn_function V2I
V2I_Construct(i32 x, i32 y)
{
    V2I v2f;
    v2f.x = x;
    v2f.y = y;
    return v2f;
}
