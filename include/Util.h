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
#include "cgltf.h"

#define PI 3.14159265359f

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

typedef struct {
    union {
        float m[16];

        float m2[4][4];

        struct {
            float m00, m01, m02, m03;
            float m10, m11, m12, m13;
            float m20, m21, m22, m23;
            float m30, m31, m32, m33;
        };
    };
} mat4;

typedef struct {
    u32* indices;
    u32 indices_count;
    vec3* vertices;
    u32 vertices_count;
}StaticMesh;

typedef struct {
    u8 is_key_down_esc;
    u8 is_key_down_w;
    u8 is_key_down_a;
    u8 is_key_down_s;
    u8 is_key_down_d;
    u8 is_key_down_space;
    u8 is_key_down_lctr;

    u8 is_key_just_pressed_esc;
    u8 is_key_just_pressed_w;
    u8 is_key_just_pressed_a;
    u8 is_key_just_pressed_s;
    u8 is_key_just_pressed_d;
    u8 is_key_just_pressed_space;
    u8 is_key_just_pressed_lctr;

    u8 is_key_released_esc;
    u8 is_key_released_w;
    u8 is_key_released_a;
    u8 is_key_released_s;
    u8 is_key_released_d;
    u8 is_key_released_space;
    u8 is_key_released_lctr;

    u8 isInFocus;
    u8 isFullscreen;
    u8 isMinimized;
    u8 isMaximized;
    i32 posX, posY;
    i32 screen_width, screen_height;

    u8 leftMousePressed;
    u8 rightMouseDown;
    u8 middleMouseReleased;
    float scrollX, scrollY;
    i32 mouseX, mouseY;
    float vectorX, vectorY;

    u8 drop;
    u8 drag;
    i32 dragX, dragY;
    const char** data;
    size_t count;
}Input;


//--------------------Draw Stuff-----------------------------
static inline void clear(u8 *buffer, i32 bufferWidth, i32 width, i32 height, u8 color[4])
{
    if (color[0] == color[1] && color[0] == color[2] && color[0] == color[3])
    {
        memset(buffer, color[0],
               (u32)bufferWidth * (u32)height * 4 * sizeof(u8));
        return;
    }

    /* else we'll have to something more complex... */
    RGFW_UNUSED(width);
    /* loop through each *pixel* (not channel) of the buffer */
    u32 x, y;
    for (y = 0; y < (u32)height; y++) {
        for (x = 0; x < (u32)bufferWidth; x++) {
            u32 index = (y * 4 * (u32)bufferWidth) + x * 4;

            /* copy the color to that pixel */
            memcpy(&buffer[index], color, 4 * sizeof(u8));
        }
    }
}

// Edge function using CCW winding order
// Source "scratchapixel.com"
static inline float EdgeFunction(const vec2 v0, const vec2 v1, const vec2 p){
    return (v0.x - v1.x) * (p.y - v0.y) - (v0.y - v1.y) * (p.x - v0.x);
}

void DrawLine(draw_buffer *buffer_info, ivec2 start, ivec2 end, float thickness,
              pixel_color line_color);

void DrawTriangle(draw_buffer* buffer,const vec2 v0, const vec2 v1, const vec2 v2);
//-------------------Draw Stuff End-------------------------------


//------------------Math Stuff------------------------------------
mat4 mat4_multiply(const mat4 a, const mat4 b);

vec3 mat4_multiply_vec3(const mat4 m, const vec3 v) ;

mat4 mat4_identity(void);

mat4 mat4_look_at(vec3 eye, vec3 center, vec3 up);

mat4 mat4_perspective(float fov_y, float aspect, float near_plane, float far_plane);

mat4 mat4_get_translated_mat4(vec3 translation);

mat4 mat4_translate(mat4 mat, vec3 translation);

mat4 mat4_scale(vec3 scale);

mat4 mat4_rotate_z(float angle_rad);

mat4 mat4_rotate_y(float angle_rad);

static inline float deg_to_rad(float degrees) {
    return degrees * (PI / 180.0f);
}

static inline vec3 normalize_vec3(vec3 vec){
    vec3 out;
    float len = sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    out.x = vec.x / len;
    out.y = vec.y / len;
    out.z = vec.z / len;
    return out;
}

static inline vec3 cross_vec3(vec3 v1, vec3 v2){
    vec3 out = {(v1.y * v2.z) - (v1.z * v2.y), (v1.z * v2.x) - (v1.x * v2.z), (v1.x * v2.y) - (v1.y * v2.x)};
    return out;
}

static inline vec3 cross_v3f(float v1x, float v1y, float v1z, float v2x, float v2y, float v2z){
    vec3 out = {(v1y * v2z) - (v1z * v2y), (v1z * v2x) - (v1x * v2z), (v1x * v2y) - (v1y * v2x)};
    return out;
}

static inline float dot_vec3(vec3 v1, vec3 v2){
    float out = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
    return out;
}
//--------------------Math Stuff End--------------------------------


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
double UpdateAndDisplayFPS(FPS_Counter* fps, RGFW_window* win);
//----------Timing Stuff End----------------


//---------Mesh Stuff----------------
StaticMesh *load_static_mesh_from_gltf(const char *file_path);

void draw_static_mesh(StaticMesh* mesh, draw_buffer* buffer, mat4 model, mat4 view, mat4 proj);
//--------Mesh Stuff End----------------


Input UpdateInput(RGFW_window* win);
