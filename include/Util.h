#pragma once
#include "RGFW.h"
#include <math.h>
#include <stdbool.h>

typedef struct
{
    i32 x;
    i32 y;
} ivec2;

typedef struct
{
    i32 x;
    i32 y;
    i32 z;
} ivec3;

typedef struct
{
    float x;
    float y;
} vec2;

typedef struct
{
    float x;
    float y;
    float z;
} vec3;

typedef struct
{
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} pixel_color;

typedef struct
{
    u8 *buffer;
    i32 size;
    i32 buffer_width;
} draw_buffer;

void IntensifyPixels(float x, float y, float distance);

void DrawLine(draw_buffer *buffer_info, ivec2 start, ivec2 end, float thickness,
              pixel_color line_color);
