#include "Util.h"


inline void SetPixel(draw_buffer *buffer_info, const i32 x, const i32 y, const pixel_color color)
{
    int index = buffer_info->buffer_width * 4 * y + x * 4;
    buffer_info->buffer[index] = color.r;
    buffer_info->buffer[index + 1] = color.g;
    buffer_info->buffer[index + 2] = color.b;
    buffer_info->buffer[index + 3] = color.a;
}

// Source: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
void DrawLine(draw_buffer *buffer_info, ivec2 start, ivec2 end, const float thickness,
              const pixel_color line_color)
{
    i32 dx = abs(end.x - start.x);
    i8 sx = start.x < end.x ? 1 : -1;
    i32 dy = -abs(end.y - start.y);
    i8 sy = start.y < end.y ? 1 : -1;

    i32 error = dx + dy;

    while (true)
    {
        SetPixel(buffer_info, start.x, start.y, line_color);
        i32 e2 = 2 * error;

        if (e2 >= dy)
        {
            if (start.x == end.x)
                break;
            error = error + dy;
            start.x = start.x + sx;
        }
        if (e2 <= dx)
        {
            if (start.y == end.y)
                break;
            error = error + dx;
            start.y = start.y + sy;
        }
    }
}

void DrawTriangle(draw_buffer* buffer,const vec2 v0, const vec2 v1, const vec2 v2){
    AABB trig_aabb = {
        .min.x = fminf(fminf(v0.x, v1.x), v2.x),
        .min.y = fminf(fminf(v0.y, v1.y), v2.y),
        .max.x = fmaxf(fmaxf(v0.x, v1.x), v2.x),
        .max.y = fmaxf(fmaxf(v0.y, v1.y), v2.y)
    };

    // Hardcoded color
    pixel_color c0 = {255, 0, 0, 255};
    pixel_color c1 = {0, 255, 0, 255};
    pixel_color c2 = {0, 0, 255, 255};

    float area = EdgeFunction(v0, v1, v2);

    for(int y = (int)trig_aabb.min.y; y <= (int)trig_aabb.max.y; y++){
        for(int x = (int)trig_aabb.min.x; x <= (int)trig_aabb.max.x; x++){
            vec2 p = {x + 0.5f, y + 0.5f};
            float w0 = EdgeFunction(v1, v2, p);
            float w1 = EdgeFunction(v2, v0, p);
            float w2 = EdgeFunction(v0, v1, p);

            if(w0 <= 0 && w1 <= 0 && w2 <= 0){
                w0 /= area;
                w1 /= area;
                w2 /= area;
                float r = w0 * c0.r + w1 * c1.r + w2 * c2.r;
                float g = w0 * c0.g + w1 * c1.g + w2 * c2.g;
                float b = w0 * c0.b + w1 * c1.b + w2 * c2.b;
                pixel_color color_changed = {r,g,b,255};
                SetPixel(buffer, x, y, color_changed);
            }
        }
    }
}

//--------------Timing Stuff--------------
void InitFPS(FPS_Counter* fps) {
    fps->accumulated_time = 0.0;
    fps->frame_count = 0;

    #ifdef _WIN32
    QueryPerformanceFrequency(&fps->frequency);
    QueryPerformanceCounter(&fps->last_time);
    #else
    clock_gettime(CLOCK_MONOTONIC, &fps->last_time);
    #endif
}

void UpdateAndDisplayFPS(FPS_Counter* fps, RGFW_window* win) {
    double elapsed_time;

    #ifdef _WIN32
    LARGE_INTEGER current_time;
    QueryPerformanceCounter(&current_time);
    elapsed_time = (double)(current_time.QuadPart - fps->last_time.QuadPart) / fps->frequency.QuadPart;
    fps->last_time = current_time;
    #else
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    elapsed_time = (current_time.tv_sec - fps->last_time.tv_sec) +
    (current_time.tv_nsec - fps->last_time.tv_nsec) / 1000000000.0;
    fps->last_time = current_time;
    #endif

    fps->accumulated_time += elapsed_time;
    fps->frame_count++;

    if (fps->accumulated_time >= 1.0) {
        char titleBuffer[64];
        snprintf(titleBuffer, sizeof(titleBuffer), "Game - FPS: %d", fps->frame_count);
        RGFW_window_setName(win, titleBuffer);

        fps->accumulated_time -= 1.0;
        fps->frame_count = 0;
    }
}
//-------------------Timing Stuff End-------------------
