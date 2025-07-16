#include "sandor.h"

#define WIDTH 400
#define HEIGHT 300
#define PI 3.14159265359

// Math functions provided by the platform
float sqrtf(float x);
float atan2f(float y, float x);
float sinf(float x);
float cosf(float x);

#include "cube.h"
#include "dvd.h"

int render_count = 0;

char* cube_canvas_id = "cube-canvas";
char* dvd_canvas_id = "dvd-canvas";

void draw_cube_canvas(float dt)
{
    cube_canvas = render_cube(dt);
    platform_draw_canvas(cube_canvas_id, &cube_canvas);
}

void draw_dvd_canvas(float dt)
{
    dvd_canvas = render_dvd(dt);
    platform_draw_canvas(dvd_canvas_id, &dvd_canvas);
}

void draw_all_canvases(float dt)
{
    draw_cube_canvas(dt);
    draw_dvd_canvas(dt);
}

void init_component() {
    printf("Initializing Canvas Component\n");
    platform_on_animation_frame(draw_all_canvases);
}

Element* render_component()
{
    render_count++;

    Element* cube_canvas_element = canvas(cube_canvas_id, 400, 300);
    Element* dvd_canvas_element = canvas(dvd_canvas_id, 400, 300);

    return class(
        element("div", children(
            class(text_element("h1", "Canvas App"), "text-3xl font-bold mb-6"),
            class(text_element("p", "Multiple canvas animations running simultaneously"), "text-lg mb-4"),
            text_element("p", arena_sprintf(&r_arena, "Render count: %d", render_count)),
            class(
                element("div", children(
                    class(
                        element("div", children(
                            class(text_element("h3", "\"3D\" Cube"), "text-xl font-semibold mb-2"),
                            class(
                                cube_canvas_element,
                                "border-2 border-gray-300 bg-white rounded-lg shadow-md"
                            )
                        )),
                        "flex flex-col items-center"
                    ),
                    class(
                        element("div", children(
                            class(text_element("h3", "DVD Screensaver"), "text-xl font-semibold mb-2"),
                            class(
                                dvd_canvas_element,
                                "border-2 border-gray-300 bg-black rounded-lg shadow-md"
                            )
                        )),
                        "flex flex-col items-center"
                    )
                )),
                "flex gap-8 flex-wrap justify-center"
            )
        )),
        "flex-1 flex flex-col items-center justify-center gap-6 p-6"
    );
}