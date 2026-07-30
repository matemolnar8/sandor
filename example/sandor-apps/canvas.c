#include "sandor.h"

#define WIDTH 400
#define HEIGHT 300

#include "cube.h"

int render_count = 0;

char* cube_canvas_id = "cube-canvas";

uint32_t cube_pixels[WIDTH*HEIGHT];
Olivec_Canvas cube_canvas = OLIVEC_CANVAS_NULL;

void draw_cube_canvas(float dt)
{
    cube_canvas = render_cube(dt, cube_pixels, WIDTH, HEIGHT);
    Canvas view = {
        .pixels = cube_canvas.pixels,
        .width = cube_canvas.width,
        .height = cube_canvas.height,
        .stride = cube_canvas.stride,
    };
    platform_draw_canvas(cube_canvas_id, &view);
}

void init_component() {
    printf("Initializing Canvas Component\n");
    platform_on_animation_frame(draw_cube_canvas);
}

Element* render_component()
{
    render_count++;

    Element* cube_canvas_element = canvas(cube_canvas_id, 400, 300);

    return class(
        element("div", children(
            class(text_element("h1", "Canvas App"), "text-3xl font-bold mb-6"),
            class(text_element("p", "Canvas animation demo"), "text-lg mb-4"),
            text_element("p", arena_sprintf(&r_arena, "Render count: %d", render_count)),
            class(
                element("div", children(
                    class(text_element("h3", "\"3D\" Cube"), "text-xl font-semibold mb-2"),
                    class(
                        cube_canvas_element,
                        "border-2 border-gray-300 bg-white rounded-lg shadow-md"
                    )
                )),
                "flex flex-col items-center"
            )
        )),
        "flex-1 flex flex-col items-center justify-center gap-6 p-6"
    );
}
