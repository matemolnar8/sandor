#include "wasm-component.h"

#define MESSAGE_CAPACITY 256

int render_count = 0;

Element* render_component()
{
    render_count++;
    char* message = arena_alloc(&render_result_arena, MESSAGE_CAPACITY);
    stbsp_sprintf(message, "Render count: %d", render_count);

    Element* result = create_element("div", children());

    Element* render_count_element = create_text_element("h1", message);
    arena_da_append(&render_result_arena, result->children, render_count_element);

    return result;
}