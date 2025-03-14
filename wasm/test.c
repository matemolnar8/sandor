#include "wasm-component.h"

#define MESSAGE_CAPACITY 256

int render_count = 0;
int count = 0;

void button_callback() {
    count++;
}

Element* render_component()
{
    render_count++;

    Children* c = children();
    Element* result = create_element("div", c);

    char* message = arena_alloc(&render_result_arena, MESSAGE_CAPACITY);
    stbsp_sprintf(message, "Count: %d", count);
    Element* count_element = create_text_element("h1", message);
    arena_da_append(&render_result_arena, result->children, count_element);

    message = arena_alloc(&render_result_arena, MESSAGE_CAPACITY);
    stbsp_sprintf(message, "Render count: %d", render_count);
    Element* render_count_element = create_text_element("p", message);
    arena_da_append(&render_result_arena, result->children, render_count_element);

    Element* button = create_button("Increment", button_callback);
    arena_da_append(&render_result_arena, result->children, button);

    return result;
}