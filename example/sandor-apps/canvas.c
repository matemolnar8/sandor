#include "sandor.h"

int render_count = 0;

// Math functions provided by the platform
float sqrtf(float x);
float atan2f(float y, float x);
float sinf(float x);
float cosf(float x);

#define WIDTH 400
#define HEIGHT 300

#define PI 3.14159265359

#define CUBE_BACKGROUND_COLOR 0xFF181818
#define CUBE_GRID_COUNT 10
#define CUBE_GRID_PAD 0.5/CUBE_GRID_COUNT
#define CUBE_GRID_SIZE ((CUBE_GRID_COUNT - 1)*CUBE_GRID_PAD)
#define CUBE_CIRCLE_RADIUS 4
#define CUBE_Z_START 0.4

uint32_t cube_pixels[WIDTH*HEIGHT];
Olivec_Canvas cube_canvas = OLIVEC_CANVAS_NULL;
static float cube_angle = 0;

// Copied from Tsoding Olive.c example
Olivec_Canvas render_cube(float dt)
{
    cube_angle += 0.25*PI*dt;

    Olivec_Canvas oc = olivec_canvas(cube_pixels, WIDTH, HEIGHT, WIDTH);

    olivec_fill(oc, CUBE_BACKGROUND_COLOR);
    for (int ix = 0; ix < CUBE_GRID_COUNT; ++ix) {
        for (int iy = 0; iy < CUBE_GRID_COUNT; ++iy) {
            for (int iz = 0; iz < CUBE_GRID_COUNT; ++iz) {
                float x = ix*CUBE_GRID_PAD - CUBE_GRID_SIZE/2;
                float y = iy*CUBE_GRID_PAD - CUBE_GRID_SIZE/2;
                float z = CUBE_Z_START + iz*CUBE_GRID_PAD;

                float cx = 0.0;
                float cz = CUBE_Z_START + CUBE_GRID_SIZE/2;

                float dx = x - cx;
                float dz = z - cz;

                float a = atan2f(dz, dx);
                float m = sqrtf(dx*dx + dz*dz);

                dx = cosf(a + cube_angle)*m;
                dz = sinf(a + cube_angle)*m;

                x = dx + cx;
                z = dz + cz;

                x /= z;
                y /= z;

                uint32_t r = ix*255/CUBE_GRID_COUNT;
                uint32_t g = iy*255/CUBE_GRID_COUNT;
                uint32_t b = iz*255/CUBE_GRID_COUNT;
                uint32_t color = 0xFF000000 | (r<<(0*8)) | (g<<(1*8)) | (b<<(2*8));
                olivec_circle(oc, (x + 1)/2*WIDTH, (y + 1)/2*HEIGHT, CUBE_CIRCLE_RADIUS, color);
            }
        }
    }

    return oc;
}

char* canvas_id = "cube-canvas";
void draw_cube_canvas(float dt)
{
    cube_canvas = render_cube(dt);
    platform_draw_canvas(canvas_id, &cube_canvas);
}

void init_component() {
    printf("Initializing Canvas Component\n");
    platform_on_animation_frame(draw_cube_canvas);
}

Element* render_component()
{
    render_count++;

    Element* canvas_element = canvas(canvas_id, 400, 300);

    return class(
        element("div", children(
            class(text_element("h1", "Canvas App"), "text-3xl font-bold mb-6"),
            class(text_element("p", "Interactive 3D cube animation"), "text-lg text-gray-600 mb-4"),
            text_element("p", arena_sprintf(&r_arena, "Render count: %d", render_count)),
            class(
                canvas_element,
                "border-2 border-gray-300 bg-white rounded-lg shadow-md"
            )
        )),
        "flex-1 flex flex-col items-center justify-center gap-4 p-6"
    );
}