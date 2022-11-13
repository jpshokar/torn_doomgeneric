#include <math/math.h>


torn_function M_Mat4x4
M_Mat4D(i32 diag)
{
    M_Mat4x4 matrix = {0};
    
    matrix.elements[0][0] = diag;
    matrix.elements[1][1] = diag;
    matrix.elements[2][2] = diag;
    matrix.elements[3][3] = diag;
    
    return matrix;
}
torn_function M_Mat4x4
M_Orthographic(r32 left,
               r32 right,
               r32 bottom,
               r32 top,
               r32 near,
               r32 far)
{
    
    M_Mat4x4 matrix = {0};
    
    matrix.elements[0][0] = 2 / (right - left);
    matrix.elements[1][1] = 2 / (top   - bottom);
    matrix.elements[2][2] = -2 / (far - near);
    
    matrix.elements[3][0] = -( (right+left) / (right-left)); 
    matrix.elements[3][1] = -( (top+bottom) / (top-bottom)); 
    matrix.elements[3][2] = -( (far+near) / (far-near)); 
    matrix.elements[3][3] = 1;
    
    
    
    return matrix;
    
}