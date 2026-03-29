#include <math.h>
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

    vec3 eye = {0.0f, 0.0f, 0.0f};
    vec3 look_at = {0.0f, 0.0f, -1.0f};
    vec3 up = {0.0f, 1.0f, 0.0f};

    mat4 model = mat4_identity();
    vec3 translation = {0, 0, 3};
    mat4 view = mat4_look_at(eye, look_at, up);
    mat4 projection = mat4_perspective(deg_to_rad(90.0f), (float)width / (float)height, 1.0f, 100.0f);

    FPS_Counter fps_counter;
    InitFPS(&fps_counter);

    StaticMesh* cube = load_static_mesh_from_gltf("assets/Cube.glb");
    float y_rotation = 0.0f;

    Input Input;

    while (running)
    {
        double delta = UpdateAndDisplayFPS(&fps_counter, win);

        Input = UpdateInput(win);
        while (RGFW_window_checkEvent(win, &event))
        {
            if (event.type == RGFW_quit ||
                RGFW_window_isKeyPressed(win, RGFW_escape))
            {
                running = 0;
                break;
            }
        }

        u8 color[4] = {100, 100, 200, 255};
        clear(buffer_info.buffer, width, width, height, color);


        model = mat4_rotate_y(y_rotation);
        model = mat4_translate(model, translation);
        draw_static_mesh(cube, &buffer_info , model, view, projection);
        y_rotation += 1.0f * delta;
        fmodf(y_rotation, (2 * PI));

        RGFW_window_blitSurface(win, surface);

    }

    RGFW_surface_free(surface);
    RGFW_free(buffer);

    RGFW_window_close(win);

    return 0;
}
