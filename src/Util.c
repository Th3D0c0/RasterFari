#include "Util.h"
#include <stdint.h>
#include <stdlib.h>

inline void SetPixel(draw_buffer *buffer_info, const i32 x, const i32 y,
                     const pixel_color color)
{
    int index = buffer_info->buffer_width * 4 * y + x * 4;
    buffer_info->buffer[index] = color.r;
    buffer_info->buffer[index + 1] = color.g;
    buffer_info->buffer[index + 2] = color.b;
    buffer_info->buffer[index + 3] = color.a;
}

// Source: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
void DrawLine(draw_buffer *buffer_info, ivec2 start, ivec2 end,
              const float thickness, const pixel_color line_color)
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

void DrawTriangle(draw_buffer *buffer, const vec2 v0, const vec2 v1,
                  const vec2 v2)
{
    AABB trig_aabb = {.min.x = fminf(fminf(v0.x, v1.x), v2.x),
                      .min.y = fminf(fminf(v0.y, v1.y), v2.y),
                      .max.x = fmaxf(fmaxf(v0.x, v1.x), v2.x),
                      .max.y = fmaxf(fmaxf(v0.y, v1.y), v2.y)};

    // Hardcoded color
    pixel_color c0 = {255, 0, 0, 255};
    pixel_color c1 = {0, 255, 0, 255};
    pixel_color c2 = {0, 0, 255, 255};

    float area = EdgeFunction(v0, v1, v2);

    for (int y = (int)trig_aabb.min.y; y <= (int)trig_aabb.max.y; y++)
    {
        for (int x = (int)trig_aabb.min.x; x <= (int)trig_aabb.max.x; x++)
        {
            vec2 p = {x + 0.5f, y + 0.5f};
            float w0 = EdgeFunction(v1, v2, p);
            float w1 = EdgeFunction(v2, v0, p);
            float w2 = EdgeFunction(v0, v1, p);

            if (w0 <= 0 && w1 <= 0 && w2 <= 0)
            {
                w0 /= area;
                w1 /= area;
                w2 /= area;
                float r = w0 * c0.r + w1 * c1.r + w2 * c2.r;
                float g = w0 * c0.g + w1 * c1.g + w2 * c2.g;
                float b = w0 * c0.b + w1 * c1.b + w2 * c2.b;
                pixel_color color_changed = {r, g, b, 255};
                SetPixel(buffer, x, y, color_changed);
            }
        }
    }
}

mat4 mat4_multiply(const mat4 a, const mat4 b)
{
    mat4 result = {0};

    // c = column index, r = row index
    for (int c = 0; c < 4; ++c)
    {
        for (int r = 0; r < 4; ++r)
        {
            result.m2[c][r] = a.m2[0][r] * b.m2[c][0] +
                              a.m2[1][r] * b.m2[c][1] +
                              a.m2[2][r] * b.m2[c][2] + a.m2[3][r] * b.m2[c][3];
        }
    }

    return result;
}

vec3 mat4_multiply_vec3(const mat4 m, const vec3 v) {
    vec3 result = {0};

    float w = 1.0f;

    float x_prime = m.m00 * v.x + m.m10 * v.y + m.m20 * v.z + m.m30 * w;
    float y_prime = m.m01 * v.x + m.m11 * v.y + m.m21 * v.z + m.m31 * w;
    float z_prime = m.m02 * v.x + m.m12 * v.y + m.m22 * v.z + m.m32 * w;
    float w_prime = m.m03 * v.x + m.m13 * v.y + m.m23 * v.z + m.m33 * w;

    if (w_prime != 0.0f) {
        result.x = x_prime / w_prime;
        result.y = y_prime / w_prime;
        result.z = z_prime / w_prime;
    }

    return result;
}

mat4 mat4_identity(void)
{
    mat4 result = {0};

    result.m00 = 1.0f;
    result.m11 = 1.0f;
    result.m22 = 1.0f;
    result.m33 = 1.0f;

    return result;
}

mat4 mat4_look_at(vec3 eye, vec3 center, vec3 up)
{
    mat4 result = {0};

    vec3 f = {center.x - eye.x, center.y - eye.y, center.z - eye.z};
    float f_len = sqrtf(f.x * f.x + f.y * f.y + f.z * f.z);
    f.x /= f_len;
    f.y /= f_len;
    f.z /= f_len;

    vec3 s = {f.y * up.z - f.z * up.y, f.z * up.x - f.x * up.z,
              f.x * up.y - f.y * up.x};
    float s_len = sqrtf(s.x * s.x + s.y * s.y + s.z * s.z);
    s.x /= s_len;
    s.y /= s_len;
    s.z /= s_len;

    vec3 u = {s.y * f.z - s.z * f.y, s.z * f.x - s.x * f.z,
              s.x * f.y - s.y * f.x};

    float dot_s_eye = (s.x * eye.x) + (s.y * eye.y) + (s.z * eye.z);
    float dot_u_eye = (u.x * eye.x) + (u.y * eye.y) + (u.z * eye.z);
    float dot_f_eye = (f.x * eye.x) + (f.y * eye.y) + (f.z * eye.z);

    result.m00 = s.x;
    result.m01 = u.x;
    result.m02 = -f.x;
    result.m03 = 0.0f;

    result.m10 = s.y;
    result.m11 = u.y;
    result.m12 = -f.y;
    result.m13 = 0.0f;

    result.m20 = s.z;
    result.m21 = u.z;
    result.m22 = -f.z;
    result.m23 = 0.0f;

    result.m30 = -dot_s_eye;
    result.m31 = -dot_u_eye;
    result.m32 = dot_f_eye;
    result.m33 = 1.0f;

    return result;
}

mat4 mat4_perspective(float fov_y, float aspect, float near_plane,
                      float far_plane)
{
    mat4 result = {0};

    float f = 1.0f / tanf(fov_y / 2.0f);

    float z_range = near_plane - far_plane;

    result.m00 = f / aspect;

    result.m11 = f;

    result.m22 = (far_plane + near_plane) / z_range;
    result.m23 = -1.0f;

    result.m32 = (2.0f * far_plane * near_plane) / z_range;

    return result;
}

mat4 mat4_get_translated_mat4(vec3 translation)
{
    mat4 result = mat4_identity();

    // Column 3 holds the translation values
    result.m30 = translation.x;
    result.m31 = translation.y;
    result.m32 = translation.z;

    return result;
}

mat4 mat4_translate(mat4 mat, vec3 translation){
    mat4 result = mat;
    result.m30 += translation.x;
    result.m31 += translation.y;
    result.m32 += translation.z;
    return result;
}

mat4 mat4_scale(vec3 scale)
{
    mat4 result = mat4_identity();

    // The main diagonal holds the scale values
    result.m00 = scale.x;
    result.m11 = scale.y;
    result.m22 = scale.z;

    return result;
}

mat4 mat4_rotate_x(float angle_rad)
{
    mat4 result = mat4_identity();
    float c = cosf(angle_rad);
    float s = sinf(angle_rad);

    result.m11 = c;
    result.m21 = -s;
    result.m12 = s;
    result.m22 = c;

    return result;
}

mat4 mat4_rotate_y(float angle_rad)
{
    mat4 result = mat4_identity();
    float c = cosf(angle_rad);
    float s = sinf(angle_rad);

    result.m00 = c;
    result.m20 = s;
    result.m02 = -s;
    result.m22 = c;

    return result;
}

mat4 mat4_rotate_z(float angle_rad)
{
    mat4 result = mat4_identity();
    float c = cosf(angle_rad);
    float s = sinf(angle_rad);

    result.m00 = c;
    result.m10 = -s;
    result.m01 = s;
    result.m11 = c;

    return result;
}

//--------------Timing Stuff--------------
void InitFPS(FPS_Counter *fps)
{
    fps->accumulated_time = 0.0;
    fps->frame_count = 0;

#ifdef _WIN32
    QueryPerformanceFrequency(&fps->frequency);
    QueryPerformanceCounter(&fps->last_time);
#else
    clock_gettime(CLOCK_MONOTONIC, &fps->last_time);
#endif
}

void UpdateAndDisplayFPS(FPS_Counter *fps, RGFW_window *win)
{
    double elapsed_time;

#ifdef _WIN32
    LARGE_INTEGER current_time;
    QueryPerformanceCounter(&current_time);
    elapsed_time = (double)(current_time.QuadPart - fps->last_time.QuadPart) /
                   fps->frequency.QuadPart;
    fps->last_time = current_time;
#else
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    elapsed_time =
        (current_time.tv_sec - fps->last_time.tv_sec) +
        (current_time.tv_nsec - fps->last_time.tv_nsec) / 1000000000.0;
    fps->last_time = current_time;
#endif

    fps->accumulated_time += elapsed_time;
    fps->frame_count++;

    if (fps->accumulated_time >= 1.0)
    {
        char titleBuffer[64];
        snprintf(titleBuffer, sizeof(titleBuffer), "Game - FPS: %d",
                 fps->frame_count);
        RGFW_window_setName(win, titleBuffer);

        fps->accumulated_time -= 1.0;
        fps->frame_count = 0;
    }
}
//-------------------Timing Stuff End-------------------

StaticMesh *load_static_mesh_from_gltf(const char *file_path)
{
    StaticMesh *sm = (StaticMesh *)malloc(sizeof(StaticMesh));

    cgltf_options options = {0};
    cgltf_data *data = NULL;
    cgltf_result result = cgltf_parse_file(&options, file_path, &data);
    if (result != cgltf_result_success)
    {
        printf("Failed to parse glTF file.\n");
        return NULL;
    }

    result = cgltf_load_buffers(&options, data, file_path);
    if (result != cgltf_result_success)
    {
        printf("Failed to load bufffer\n");
        cgltf_free(data);
        return NULL;
    }

    for (cgltf_size m = 0; m < data->meshes_count; m++)
    {
        cgltf_mesh *mesh = &data->meshes[m];

        for (cgltf_size p = 0; p < mesh->primitives_count; p++)
        {
            cgltf_primitive *primitive = &mesh->primitives[p];

            if (primitive->indices != NULL)
            {
                cgltf_accessor *index_accessor = primitive->indices;

                sm->indices_count = index_accessor->count;
                sm->indices = (u32 *)malloc(sm->indices_count * sizeof(u32));

                for (cgltf_size i = 0; i < index_accessor->count; i++)
                {
                    sm->indices[i] =
                        cgltf_accessor_read_index(index_accessor, i);
                }

                for (cgltf_size a = 0; a < primitive->attributes_count; a++)
                {
                    cgltf_attribute *attribute = &primitive->attributes[a];

                    if (attribute->type == cgltf_attribute_type_position)
                    {
                        cgltf_accessor *accessor = attribute->data;

                        if (accessor->type == cgltf_type_vec3 &&
                            accessor->component_type ==
                                cgltf_component_type_r_32f)
                        {
                            sm->vertices_count = accessor->count;
                            sm->vertices = (vec3 *)malloc(sm->vertices_count *
                                                          sizeof(vec3));

                            if (sm->vertices != NULL)
                            {
                                for (cgltf_size i = 0; i < accessor->count; i++)
                                {
                                    cgltf_accessor_read_float(
                                        accessor, i, (float *)&sm->vertices[i],
                                        3);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    cgltf_free(data);
    return sm;
}

void draw_static_mesh(StaticMesh* mesh, draw_buffer* buffer, mat4 model, mat4 view, mat4 proj) {
    mat4 mvp = mat4_multiply(mat4_multiply(proj, view), model);

    i32 width = buffer->buffer_width;
    i32 height = buffer->size / (width * 4);

    for (size_t i = 0; i < mesh->indices_count; i += 3) {
        uint32_t i1 = mesh->indices[i];
        uint32_t i2 = mesh->indices[i+1];
        uint32_t i3 = mesh->indices[i+2];

        vec3 v1 = mesh->vertices[i1];
        vec3 v2 = mesh->vertices[i2];
        vec3 v3 = mesh->vertices[i3];

        // Transform to NDC
        v1 = mat4_multiply_vec3(mvp, v1);
        v2 = mat4_multiply_vec3(mvp, v2);
        v3 = mat4_multiply_vec3(mvp, v3);

        // Map NDC to Screen Space Pixels
        vec2 v1_2d;
        v1_2d.x = (v1.x + 1.0f) * 0.5f * width;
        v1_2d.y = (1.0f - v1.y) * 0.5f * height;

        vec2 v2_2d;
        v2_2d.x = (v2.x + 1.0f) * 0.5f * width;
        v2_2d.y = (1.0f - v2.y) * 0.5f * height;

        vec2 v3_2d;
        v3_2d.x = (v3.x + 1.0f) * 0.5f * width;
        v3_2d.y = (1.0f - v3.y) * 0.5f * height;

        DrawTriangle(buffer, v1_2d, v2_2d, v3_2d);
    }
}
