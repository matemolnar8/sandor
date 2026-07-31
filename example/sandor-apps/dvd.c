#include "sandor.h"
#include "dvd.h"

#define WIDTH 640
#define HEIGHT 480

char* dvd_canvas_id = "dvd-canvas";

uint32_t dvd_pixels[WIDTH * HEIGHT];
Olivec_Canvas dvd_canvas = OLIVEC_CANVAS_NULL;

void draw_dvd_canvas(float dt)
{
    dvd_canvas = render_dvd(dt, dvd_pixels, WIDTH, HEIGHT);
    Canvas view = {
        .pixels = dvd_canvas.pixels,
        .width = dvd_canvas.width,
        .height = dvd_canvas.height,
        .stride = dvd_canvas.stride,
    };
    platform_draw_canvas(dvd_canvas_id, &view);
}

void init_component() {
    printf("Initializing DVD Screensaver\n");
    platform_on_animation_frame(draw_dvd_canvas);
}

Element* render_component()
{
    return class(
        element("div", children(
            class(text_element("h1", "DVD Screensaver"), "text-3xl font-bold mb-6"),
            class(
                canvas(dvd_canvas_id, WIDTH, HEIGHT),
                "border-2 border-gray-300 bg-black rounded-lg shadow-md"
            )
        )),
        "flex-1 flex flex-col items-center justify-center gap-6 p-6"
    );
}
