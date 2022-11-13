

#ifndef MATH_H
#define MATH_H
#include <base/base.h>

typedef struct M_Mat4x4 M_Mat4x4;
struct M_Mat4x4
{
    r32 elements[4][4];
};

torn_function M_Mat4x4 M_Mat4D(i32 diag);
torn_function M_Mat4x4 M_Orthographic(r32 left,
                                      r32 right,
                                      r32 bottom,
                                      r32 top,
                                      r32 near,
                                      r32 far);



#endif 
