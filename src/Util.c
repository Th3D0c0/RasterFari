#include "Util.h"

void IntensifyPixels(float x, float y, float distance)
{
}

void SetPixel(draw_buffer *buffer_info, i32 x, i32 y, pixel_color color)
{
    int index = buffer_info->buffer_width * 4 * y + x * 4;
    buffer_info->buffer[index] = color.r;
    buffer_info->buffer[index + 1] = color.g;
    buffer_info->buffer[index + 2] = color.b;
    buffer_info->buffer[index + 3] = color.a;
}

// Source: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
void DrawLine(draw_buffer *buffer_info, ivec2 start, ivec2 end, float thickness,
              pixel_color line_color)
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
