#include "sandor.h"

int render_count = 0;
int count = 0;

void button_callback(void* args) {
    ASSERT(args == NULL);
    count++;
}

Element* render_component()
{
    render_count++;

    return class(
        element("div", children(
            class(text_element("h1", "Hello, world!"), "text-3xl font-bold"),
            class(text_element("p", arena_sprintf(&r_arena, "Count: %d", count)), "text-xl"),
            text_element("p", arena_sprintf(&r_arena, "Render count: %d", render_count)),
            class(
                button("Increment", button_callback, NULL),
                "btn rounded-full"
            )
        )),
        "flex-1 flex flex-col items-center gap-4 p-6"
    );
}