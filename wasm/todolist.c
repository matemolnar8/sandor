#include "wasm-component.h"

Element* render_component()
{
    return element("div", children(
        text_element_with_text("h1", "To-Do list")
    ));
}