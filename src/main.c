#include <bits/time.h>
#define RGFWDEF extern
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "RGFW.h"
#include "Util.h"

void clear(u8 *buffer, i32 bufferWidth, i32 width, i32 height, u8 color[4])
{
    if (color[0] == color[1] && color[0] == color[2] && color[0] == color[3])
    {
        memset(buffer, color[0],
               (u32)bufferWidth * (u32)height * 4 * sizeof(u8));
    }
}

int main()
{
    RGFW_window *win =
        RGFW_createWindow("RasterFari", 0, 0, 500, 500,
                          RGFW_windowCenter | RGFW_windowTransparent);
    RGFW_window_setExitKey(win, RGFW_escape);

    RGFW_monitor *mon = RGFW_window_getMonitor(win);
    i32 width = 500;
    i32 height = 500;

    if (mon)
    {
        width = (i32)((float)mon->mode.w * mon->pixelRatio);
        height = (i32)((float)mon->mode.h * mon->pixelRatio);
    }

    u8 *buffer = (u8 *)RGFW_alloc((u32)(width * height * 4));
    RGFW_surface *surface =
        RGFW_createSurface(buffer, width, height, RGFW_formatRGBA8);

    i8 running = 1;

    RGFW_event event;
    float theta = 0;

    draw_buffer buffer_info = {
        .buffer = buffer,
        .size = width * height * 4,
        .buffer_width = width,
    };

    i32 mouseX = 0.0f;
    i32 mouseY = 0.0f;

    struct timespec last_time, current_time;
    clock_gettime(CLOCK_MONOTONIC, &last_time);
    double elapsed_time;
    double accumulated_time = 0.0;
    int frame_count = 0;

    while (running)
    {

        while (RGFW_window_checkEvent(win, &event))
        {
            if (event.type == RGFW_quit ||
                RGFW_window_isKeyPressed(win, RGFW_escape))
            {
                running = 0;
                break;
            }
        }
        RGFW_pollEvents();
        RGFW_window_getMouse(win, &mouseX, &mouseY);

        i32 w;
        i32 h;
        RGFW_window_getSize(win, &w, &h);

        u8 color[4] = {255, 255, 255, 255};
        clear(buffer_info.buffer, width, w, h, color);

        pixel_color line_color = {0, 0, 0, 255};
        DrawLine(&buffer_info, (ivec2){.x = w / 2, .y = h / 2},
                 (ivec2){.x = mouseX, .y = mouseY}, 3.0f, line_color);

        //--------Calculate FPS--------------
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        elapsed_time =
            (current_time.tv_sec - last_time.tv_sec) +
            (current_time.tv_nsec - last_time.tv_nsec) / 1000000000.0;
        last_time = current_time;

        accumulated_time += elapsed_time;
        frame_count++;

        if (accumulated_time >= 1.0)
        {
            char titleBuffer[64];
            snprintf(titleBuffer, sizeof(titleBuffer), "Game - FPS: %d",
                     frame_count);

            RGFW_window_setName(win, titleBuffer);
            accumulated_time -= 1.0;
            frame_count = 0;
        }
        //------------------------------------

        RGFW_window_blitSurface(win, surface);
    }

    RGFW_surface_free(surface);
    RGFW_free(buffer);

    RGFW_window_close(win);

    return 0;
}
