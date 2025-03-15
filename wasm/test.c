#include "wasm-component.h"

int render_count = 0;
int count = 0;

void button_callback() {
    count++;
}

Element* render_component()
{
    render_count++;

    Element* hello_world_element = create_text_element_with_text("h1", "Hello, world!");

    Element* count_element = create_text_element("h2");
    stbsp_sprintf(count_element->text, "Count: %d", count);

    Element* render_count_element = create_text_element("p");
    stbsp_sprintf(render_count_element->text, "Render count: %d", render_count);

    Element* button = create_button("Increment", button_callback);

    Element* result = create_element("div", children());
    arena_da_append(&render_result_arena, result->children, hello_world_element);
    arena_da_append(&render_result_arena, result->children, count_element);
    arena_da_append(&render_result_arena, result->children, render_count_element);
    arena_da_append(&render_result_arena, result->children, button);

    return result;
}