#include "sandor.h"

int render_count = 0;

#define WIDTH 400
#define HEIGHT 300
uint32_t pixels[WIDTH*HEIGHT];

Olivec_Canvas demo_canvas = OLIVEC_CANVAS_NULL;

void init_component() {
    printf("Initializing Canvas Component\n");
    demo_canvas = olivec_canvas(pixels, 400, 300, 400);
}

float sqrtf(float x);
float atan2f(float y, float x);
float sinf(float x);
float cosf(float x);

#define PI 3.14159265359

#define BACKGROUND_COLOR 0xFF181818
#define GRID_COUNT 10
#define GRID_PAD 0.5/GRID_COUNT
#define GRID_SIZE ((GRID_COUNT - 1)*GRID_PAD)
#define CIRCLE_RADIUS 5
#define Z_START 0.25

static float angle = 0;

// Copied from Tsoding olive.c example
Olivec_Canvas vc_render(float dt)
{
    angle += 0.25*PI*dt;

    Olivec_Canvas oc = olivec_canvas(pixels, WIDTH, HEIGHT, WIDTH);

    olivec_fill(oc, BACKGROUND_COLOR);
    for (int ix = 0; ix < GRID_COUNT; ++ix) {
        for (int iy = 0; iy < GRID_COUNT; ++iy) {
            for (int iz = 0; iz < GRID_COUNT; ++iz) {
                float x = ix*GRID_PAD - GRID_SIZE/2;
                float y = iy*GRID_PAD - GRID_SIZE/2;
                float z = Z_START + iz*GRID_PAD;

                float cx = 0.0;
                float cz = Z_START + GRID_SIZE/2;

                float dx = x - cx;
                float dz = z - cz;

                float a = atan2f(dz, dx);
                float m = sqrtf(dx*dx + dz*dz);

                dx = cosf(a + angle)*m;
                dz = sinf(a + angle)*m;

                x = dx + cx;
                z = dz + cz;

                x /= z;
                y /= z;

                uint32_t r = ix*255/GRID_COUNT;
                uint32_t g = iy*255/GRID_COUNT;
                uint32_t b = iz*255/GRID_COUNT;
                uint32_t color = 0xFF000000 | (r<<(0*8)) | (g<<(1*8)) | (b<<(2*8));
                olivec_circle(oc, (x + 1)/2*WIDTH, (y + 1)/2*HEIGHT, CIRCLE_RADIUS, color);
            }
        }
    }

    return oc;
}

char* canvas_id = "demo-canvas";

void draw_canvas(void* args)
{
    ASSERT(args == NULL);

    float dt = 0.064; // Simulate a frame time of 64ms (15.625 FPS)

    demo_canvas = vc_render(dt);

    platform_draw_canvas(canvas_id, &demo_canvas);
}

Element* render_component()
{
    render_count++;

    Element* canvas_element = canvas(canvas_id, 400, 300);

    return class(
        element("div", children(
            class(text_element("h1", "Canvas App"), "text-3xl font-bold mb-6"),
            class(text_element("p", "A simple canvas element for drawing"), "text-lg text-gray-600 mb-4"),
            text_element("p", arena_sprintf(&r_arena, "Render count: %d", render_count)),
            class(
                canvas_element,
                "border-2 border-gray-300 bg-white rounded-lg shadow-md"
            ),
            class(button("Draw", draw_canvas, NULL), "btn btn-primary mt-4")
        )),
        "flex-1 flex flex-col items-center justify-center gap-4 p-6"
    );
}