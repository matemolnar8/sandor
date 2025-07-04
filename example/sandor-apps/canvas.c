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

void draw_canvas(void* args)
{
    ASSERT(args == NULL);

    olivec_fill(demo_canvas, 0xFFFFFFFF);

    size_t rect_x = demo_canvas.width / 4;
    size_t rect_y = demo_canvas.height / 4;
    size_t rect_width = demo_canvas.width / 2;
    size_t rect_height = demo_canvas.height / 2;

    olivec_rect(demo_canvas, rect_x, rect_y, rect_width, rect_height, 0xFF0000FF);
    olivec_circle(demo_canvas, demo_canvas.width / 2, demo_canvas.height / 2, 50, 0xFF00FF00);

    platform_draw_canvas(&demo_canvas);
}

Element* render_component()
{
    render_count++;

    Element* canvas_element = canvas("demo-canvas", 400, 300);

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