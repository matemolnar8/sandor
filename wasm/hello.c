#include "wasm-component.h"

int render_count = 0;

Element* render_component()
{
    render_count++;

    Element* result = arena_alloc(&render_result_arena, sizeof(Element));

    char* message = arena_alloc(&render_result_arena, WRITE_BUFFER_CAPACITY);
    stbsp_sprintf(message, "Render count: %d", render_count);

    Element* text_element = create_text_element(message, "h1");
    result->children = arena_alloc(&render_result_arena, sizeof(Children));
    result->children->count = 0;
    arena_da_append(&render_result_arena, result->children, text_element);

    result->type = "div";

    return result;
}