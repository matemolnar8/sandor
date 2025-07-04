#include "sandor.h"

int render_count = 0;

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
            )
        )),
        "flex-1 flex flex-col items-center justify-center gap-4 p-6"
    );
}