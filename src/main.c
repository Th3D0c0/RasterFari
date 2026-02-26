#include <stdio.h>

#include "RGFW.h"
#include "Util.h"

int main()
{
    setvbuf(stdout, NULL, _IONBF, 0);

    RGFW_window *win =
        RGFW_createWindow("RasterFari", 0, 0, 800, 600,
                          RGFW_windowCenter | RGFW_windowTransparent);
    RGFW_window_setExitKey(win, RGFW_escape);

    RGFW_monitor *mon = RGFW_window_getMonitor(win);
    i32 width = 800;
    i32 height = 600;

    u8 *buffer = (u8 *)RGFW_alloc((u32)(width * height * 4));
    RGFW_surface *surface =
        RGFW_createSurface(buffer, width, height, RGFW_formatRGBA8);

    i8 running = 1;

    RGFW_event event;

    draw_buffer buffer_info = {
        .buffer = buffer,
        .size = width * height * 4,
        .buffer_width = width,
    };

    i32 mouseX = 0;
    i32 mouseY = 0;

    FPS_Counter fps_counter;
    InitFPS(&fps_counter);

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

        u8 color[4] = {255, 255, 255, 255};
        clear(buffer_info.buffer, width, width, height, color);

        vec2 trig_v1 = {200,300};
        vec2 trig_v2 = {400, 200};
        vec2 trig_v3 = {600, 300};
        DrawTriangle(&buffer_info, trig_v1, trig_v2, trig_v3);

        UpdateAndDisplayFPS(&fps_counter, win);

        RGFW_window_blitSurface(win, surface);
    }

    RGFW_surface_free(surface);
    RGFW_free(buffer);

    RGFW_window_close(win);

    return 0;
}
