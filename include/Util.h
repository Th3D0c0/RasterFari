#pragma once

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#else
#include <time.h>
#endif

#include "RGFW.h"


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

typedef struct
{
    vec2 min;
    vec2 max;
} AABB;

static inline void clear(u8 *buffer, i32 bufferWidth, i32 width, i32 height, u8 color[4])
{
    if (color[0] == color[1] && color[0] == color[2] && color[0] == color[3])
    {
        memset(buffer, color[0],
               (u32)bufferWidth * (u32)height * 4 * sizeof(u8));
    }
}

void DrawLine(draw_buffer *buffer_info, ivec2 start, ivec2 end, float thickness,
              pixel_color line_color);

// Edge function using CCW winding order
// Source "scratchapixel.com"
static inline float EdgeFunction(const vec2 v0, const vec2 v1, const vec2 p){
    return (v0.x - v1.x) * (p.y - v0.y) - (v0.y - v1.y) * (p.x - v0.x);
}

void DrawTriangle(draw_buffer* buffer,const vec2 v0, const vec2 v1, const vec2 v2);

//----------- Timing Stuff ----------------
typedef struct {
    #ifdef _WIN32
    LARGE_INTEGER frequency;
    LARGE_INTEGER last_time;
    #else
    struct timespec last_time;
    #endif
    double accumulated_time;
    int frame_count;
} FPS_Counter;

// Initializes the timer state
void InitFPS(FPS_Counter* fps);

// Updates the time, increments frames, and updates the window title every second
void UpdateAndDisplayFPS(FPS_Counter* fps, RGFW_window* win);

//----------Timing Stuff End----------------
