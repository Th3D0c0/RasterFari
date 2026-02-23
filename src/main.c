#define RGFW_IMPLEMENTATION
#include "RGFW.h"
#include <stdio.h>

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
        i32 w;
        i32 h;
        RGFW_window_getSize(win, &w, &h);

        u8 color[4] = {255, 255, 255, 255};
        clear(buffer, width, w, h, color);

        RGFW_window_blitSurface(win, surface);
    }

    RGFW_surface_free(surface);
    RGFW_free(buffer);

    RGFW_window_close(win);

    return 0;
}
