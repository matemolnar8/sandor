#include <stdbool.h>
#include "sandor.h"
#include "cube.h"

typedef struct {
    char* title;
    char* background_class;
    Element* content;
} Slide;

// Example component
int click_count = 0;

void example_component_click(void* args) {
    ASSERT(args == NULL);
    click_count++;
}

Element* render_example_component() {
    char* button_text = arena_sprintf(&r_arena, "Clicked %d times", click_count);
    
    return class(
        element("div", children(
            class(text_element("h1", "Hello World"), "text-center"),
            class(button(button_text, example_component_click, NULL), "btn btn-primary")
        )),
        "flex flex-col items-center gap-4"
    );
}
// End of example component

// Demo canvas
#define DEMO_WIDTH 300
#define DEMO_HEIGHT 300

char* demo_canvas_id = "demo-canvas";
uint32_t demo_cube_pixels[DEMO_WIDTH*DEMO_HEIGHT];
Olivec_Canvas demo_cube_canvas = OLIVEC_CANVAS_NULL;

void draw_demo_canvas(float dt) {
    demo_cube_canvas = render_cube(dt, demo_cube_pixels, DEMO_WIDTH, DEMO_HEIGHT);
    platform_draw_canvas(demo_canvas_id, &demo_cube_canvas);
}
// End of demo canvas

// Common slide styles
#define SLIDE_BASE_CLASSES "bg-base-100 p-8 rounded-2xl shadow-xl"
#define TEXT_SLIDE_CLASSES "text-2xl text-center leading-relaxed whitespace-pre-line " SLIDE_BASE_CLASSES
#define CODE_SLIDE_CLASSES "font-mono overflow-auto p-8 rounded-2xl shadow-xl " SLIDE_BASE_CLASSES

Slide get_slide(size_t slide_index) {
    switch (slide_index) {
        case 0: // Title slide
            return (Slide) {
                .title = "🚁 sandor.h",
                .background_class = "bg-base-200",
                .content = class(
                    element("div", children(
                        text_element("p", "Write web UIs in C")
                    )),
                    TEXT_SLIDE_CLASSES
                )
            };
        
        case 1: // What is? slide
            return (Slide) {
                .title = "What is sandor.h?",
                .background_class = "bg-base-200",
                .content = class(
                    element("div", children(
                        class(element("ul", children(
                            text_element("li", "Single-header* library for web UIs in C"),
                            text_element("li", "Recreational programming project"),
                            text_element("li", "Powers this presentation")
                        )), "list-disc list-inside text-left")
                    )),
                    TEXT_SLIDE_CLASSES
                )
            };
        
        case 2: // Architecture slide
            return (Slide) {
                .title = "🏗️ Architecture",
                .background_class = "bg-base-200",
                .content = class(
                    element("div", children(
                        text_element("pre", 
                            "┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐\n"
                            "│   C Application │    │      clang      │    │   WebAssembly   │\n"
                            "│   + sandor.h    │────│       +         │────│     (.wasm)     │\n"
                            "│                 │    │      nob        │    │                 │\n"
                            "└─────────────────┘    └─────────────────┘    └─────────────────┘\n"
                            "                                                        │\n"
                            "                                                        │\n"
                            "┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐\n"
                            "│   Styled DOM    │────│  JS Bridge      │────│  WASM Runtime   │\n"
                            "│  (HTML + CSS)   │    │  (TypeScript)   │    │   (Browser)     │\n"
                            "└─────────────────┘    └─────────────────┘    └─────────────────┘\n"
                            "         │\n"
                            "         │\n"
                            "┌─────────────────┐\n"
                            "│  Tailwind CSS   │\n"
                            "│   (Styling)     │\n"
                            "└─────────────────┘"
                        )
                    )),
                    CODE_SLIDE_CLASSES
                )
            };
        
        case 3: // Code slide
            return (Slide) {
                .title = "💻 Example component",
                .background_class = "bg-base-200",
                .content = class(
                    element("div", children(
                        // Code example
                        class(
                            element("div", children(
                                class(text_element("h2", "Code"), "text-xl font-bold mb-4"),
                                // This should always reflect the actual code in the example component, except for the function name as that should be render_component for the example to be valid
                                text_element("pre", "#include \"sandor.h\"\n"
                                                   "\n"
                                                   "int click_count = 0;\n"
                                                   "\n"
                                                   "void example_component_click(void* args) {\n"
                                                   "    ASSERT(args == NULL);\n"
                                                   "    click_count++;\n"
                                                   "}\n"
                                                   "\n"
                                                   "Element* render_component() {\n"
                                                   "    char* button_text = arena_sprintf(&r_arena, \"Clicked %d times\", click_count);\n"
                                                   "    \n"
                                                   "    return class(\n"
                                                   "        element(\"div\", children(\n"
                                                   "            class(text_element(\"h1\", \"Hello World\"), \"text-center\"),\n"
                                                   "            class(button(button_text, example_component_click, NULL), \"btn btn-primary\")\n"
                                                   "        )),\n"
                                                   "        \"flex flex-col items-center gap-4\"\n"
                                                   "    );\n"
                                                   "}")
                            )),
                            "text-sm flex-1 " CODE_SLIDE_CLASSES
                        ),
                        // Interactive example
                        class(
                            element("div", children(
                                render_example_component()
                            )),
                            "flex flex-col items-center justify-center " SLIDE_BASE_CLASSES
                        )
                    )),
                    "flex gap-6 w-full"
                )
            };
        
        case 4: // Canvas slide
            return (Slide) {
                .title = "🎨 Canvas example",
                .background_class = "bg-base-200",
                .content = class(
                    element("div", children(
                        class(
                            element("div", children(
                                class(text_element("h2", "Code"), "text-xl font-bold mb-4"),
                                text_element("pre", "#include \"sandor.h\"\n"
                                                   "#include \"cube.h\"\n"
                                                   "\n"
                                                   "#define WIDTH 400\n"
                                                   "#define HEIGHT 400\n"
                                                   "\n"
                                                   "char* canvas_id = \"my-canvas\";\n"
                                                   "uint32_t pixels[WIDTH*HEIGHT];\n"
                                                   "Olivec_Canvas canvas;\n"
                                                   "\n"
                                                   "void draw_canvas(float dt) {\n"
                                                   "    canvas = render_cube(dt, pixels, WIDTH, HEIGHT);\n"
                                                   "    platform_draw_canvas(canvas_id, &canvas);\n"
                                                   "}\n"
                                                   "\n"
                                                   "void init_component() {\n"
                                                   "    platform_on_animation_frame(draw_canvas);\n"
                                                   "}\n"
                                                   "\n"
                                                   "Element* render_component() {\n"
                                                   "    return attributes(element(\"canvas\", NULL),\n"
                                                   "        \"id\", canvas_id,\n"
                                                   "        \"width\", \"400\", \"height\", \"400\");\n"
                                                   "}")
                            )),
                            "text-sm flex-1 " CODE_SLIDE_CLASSES
                        ),
                        class(
                            element("div", children(
                                attributes(
                                    element("canvas", NULL),
                                    "id", "demo-canvas",
                                    "width", "300", "height", "300"
                                )
                            )),
                            "flex flex-col items-center justify-center " SLIDE_BASE_CLASSES
                        )
                    )),
                    "flex gap-6 w-full"
                )
            };
        
        case 5: // Future plans slide
            return (Slide) {
                .title = "What's next?",
                .background_class = "bg-base-200",
                .content = class(
                    element("div", children(
                        class(element("ul", children(
                            text_element("li", "⌨️ Keyboard events"),
                            text_element("li", "📦 Actually single-header"),
                            text_element("li", "🔌 Standalone TS bridge"),
                            text_element("li", "🚁 Continue having fun programming")
                        )), "list-none text-left")
                    )),
                    TEXT_SLIDE_CLASSES
                )
            };
        
        case 6: // Thank you slide
            return (Slide) {
                .title = "Thanks!",
                .background_class = "bg-success",
                .content = class(
                    element("div", children(
                        text_element("p", "Any questions?"),
                        class(
                            element("a", children(
                                text_element("span", "github.com/matemolnar8/sandor")
                            )),
                            "link"
                        )
                    )),
                    TEXT_SLIDE_CLASSES
                )
            };
        
        default:
            return (Slide) {
                .title = "Unknown Slide",
                .background_class = "bg-base-200",
                .content = text_element("p", "Slide not found")
            };
    }
}

#define SLIDE_COUNT 7
size_t current_slide = 0;

void next_slide(void* args) {
    ASSERT(args == NULL);
    if (current_slide < SLIDE_COUNT - 1) {
        current_slide++;
    }
}

void prev_slide(void* args) {
    ASSERT(args == NULL);
    if (current_slide > 0) {
        current_slide--;
    }
}

void goto_first_slide(void* args) {
    ASSERT(args == NULL);
    current_slide = 0;
}

void goto_last_slide(void* args) {
    ASSERT(args == NULL);
    current_slide = SLIDE_COUNT - 1;
}

Element* slide_navigation() {
    char* slide_counter = arena_sprintf(&r_arena, "%zu / %d", current_slide + 1, SLIDE_COUNT);
    
    return class(
        element("div", children(
            class(
                element("div", children(
                    class(button("⏮", goto_first_slide, NULL), current_slide == 0 ? "btn btn-circle btn-disabled" : "btn btn-circle btn-primary"),
                    class(button("◀", prev_slide, NULL), current_slide == 0 ? "btn btn-circle btn-disabled" : "btn btn-circle btn-secondary"),
                    class(text_element("span", slide_counter), "mx-6 text-lg font-bold badge badge-neutral badge-lg"),
                    class(button("▶", next_slide, NULL), current_slide >= SLIDE_COUNT - 1 ? "btn btn-circle btn-disabled" : "btn btn-circle btn-secondary"),
                    class(button("⏭", goto_last_slide, NULL), current_slide >= SLIDE_COUNT - 1 ? "btn btn-circle btn-disabled" : "btn btn-circle btn-primary")
                )),
                "flex items-center justify-center gap-3"
            )
        )),
        "fixed bottom-8 left-1/2 transform -translate-x-1/2 bg-base-100 rounded-2xl px-8 py-4 shadow-2xl"
    );
}

Element* slide_content(Slide slide) {
    if (current_slide >= SLIDE_COUNT) {
        current_slide = 0;
    }
    
    return class(
        element("div", children(
            class(
                text_element("h1", slide.title), 
                "text-7xl font-black mb-12 text-center rounded-lg shadow-lg bg-base-100 p-6"
            ),
            slide.content
        )),
        "flex flex-col items-center justify-center flex-1 p-8 mx-auto"
    );
}

Element* render_component() {
    Slide slide = get_slide(current_slide);

    if(current_slide == 4) {
        // Ensure the demo canvas is drawn on the canvas slide
        platform_on_animation_frame(draw_demo_canvas);
    } else {
        // Clear any previous animation frame callbacks
        platform_clear_animation_frame(draw_demo_canvas);
    }
    
    return class(
        element("div", children(
            slide_content(slide),
            slide_navigation()
        )),
        arena_sprintf(&r_arena, "h-full w-full flex flex-col transition-all duration-500 ease-in-out %s", slide.background_class)
    );
}
