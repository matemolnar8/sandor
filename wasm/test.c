#include "wasm-component.h"

int render_count = 0;
int count = 0;

void button_callback() {
    count++;
}

Element* render_component()
{
    render_count++;

    Element* count_element = text_element("h2");
    stbsp_sprintf(count_element->text, "Count: %d", count);

    Element* render_count_element = text_element("p");
    stbsp_sprintf(render_count_element->text, "Render count: %d", render_count);

    return element("div", children(
        text_element_with_text("h1", "Hello, world!"),
        count_element,
        render_count_element,
        button("Increment", button_callback)
    ));
}