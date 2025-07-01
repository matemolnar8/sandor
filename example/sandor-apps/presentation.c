#include <stdbool.h>
#include "sandor.h"

#define SLIDE_COUNT 6

typedef struct {
    char* title;
    char* background_class;
    Element* content;
} Slide;

size_t current_slide = 0;

// Common slide styles
#define SLIDE_BASE_CLASSES "text-base-content bg-base-100 bg-opacity-10 p-8 rounded-2xl backdrop-blur-sm border border-base-content border-opacity-10 shadow-xl"
#define TEXT_SLIDE_CLASSES "text-2xl text-center leading-relaxed whitespace-pre-line " SLIDE_BASE_CLASSES
#define CODE_SLIDE_CLASSES "bg-opacity-20 font-mono text-left overflow-auto border-opacity-20 shadow-2xl " SLIDE_BASE_CLASSES

Slide get_slide(size_t slide_index) {
    switch (slide_index) {
        case 0: // Title slide
            return (Slide) {
                .title = "🚁 sandor.h",
                .background_class = "bg-gradient-to-br from-primary/20 to-secondary/20",
                .content = class(
                    element("div", children(
                        text_element("p", "WebAssembly-based C framework for web development")
                    )),
                    TEXT_SLIDE_CLASSES
                )
            };
        
        case 1: // What is? slide
            return (Slide) {
                .title = "What is Sandor?",
                .background_class = "bg-gradient-to-br from-accent/20 to-info/20",
                .content = class(
                    element("div", children(
                        class(element("ul", children(
                            class(text_element("li", "Single-header* library for creating Web UIs in C 🧱"), "mb-2"),
                            class(text_element("li", "Recreational Programming Project 🚀"), "mb-2"),
                            class(text_element("li", "The thing that powers this presentation ⚡"), "mb-2")
                        )), "list-disc list-inside text-left")
                    )),
                    TEXT_SLIDE_CLASSES
                )
            };
        
        case 2: // Architecture slide
            return (Slide) {
                .title = "🏗️ Architecture",
                .background_class = "bg-gradient-to-br from-success/20 to-warning/20",
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
                .title = "💻 Example Component",
                .background_class = "bg-gradient-to-br from-neutral/20 to-base-300/30",
                .content = class(
                    element("div", children(
                        text_element("pre", "Element* render_component() {\n"
                                           "    return class(\n"
                                           "        element(\"div\", children(\n"
                                           "            text_element(\"h1\", \"Hello World\"),\n"
                                           "            button(\"Click me\", on_click, NULL)\n"
                                           "        )),\n"
                                           "        \"flex flex-col items-center\"\n"
                                           "    );\n"
                                           "}")
                    )),
                    CODE_SLIDE_CLASSES
                )
            };
        
        case 4: // Benefits slide
            return (Slide) {
                .title = "🎯 Why Choose Sandor?",
                .background_class = "bg-gradient-to-br from-secondary/20 to-primary/20",
                .content = class(
                    element("div", children(
                        text_element("p", "⚡ Near-native performance"),
                        text_element("p", "🛡️ Memory safety"),
                        text_element("p", "🔧 Familiar C syntax"),
                        text_element("p", "📱 Modern web technologies"),
                        text_element("p", "🎨 Beautiful UIs with Tailwind CSS")
                    )),
                    TEXT_SLIDE_CLASSES
                )
            };
        
        case 5: // Thank you slide
            return (Slide) {
                .title = "🙏 Thank You!",
                .background_class = "bg-gradient-to-br from-primary/20 via-accent/15 to-secondary/20",
                .content = class(
                    element("div", children(
                        text_element("p", "Questions?"),
                        text_element("br", ""),
                        text_element("p", "🚀 Build amazing web apps with Sandor today!"),
                        text_element("br", ""),
                        text_element("p", "⭐ Star us on GitHub!")
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
                    class(button("⏮", goto_first_slide, NULL), current_slide == 0 ? "btn btn-circle btn-disabled opacity-50" : "btn btn-circle btn-primary"),
                    class(button("◀", prev_slide, NULL), current_slide == 0 ? "btn btn-circle btn-disabled opacity-50" : "btn btn-circle btn-secondary"),
                    class(text_element("span", slide_counter), "mx-6 text-lg font-bold text-base-content bg-base-100 bg-opacity-80 px-4 py-2 rounded-full"),
                    class(button("▶", next_slide, NULL), current_slide >= SLIDE_COUNT - 1 ? "btn btn-circle btn-disabled opacity-50" : "btn btn-circle btn-secondary"),
                    class(button("⏭", goto_last_slide, NULL), current_slide >= SLIDE_COUNT - 1 ? "btn btn-circle btn-disabled opacity-50" : "btn btn-circle btn-primary")
                )),
                "flex items-center justify-center gap-3"
            )
        )),
        "fixed bottom-8 left-1/2 transform -translate-x-1/2 bg-base-100 bg-opacity-95 backdrop-blur-lg rounded-2xl px-8 py-4 shadow-2xl border border-base-content border-opacity-10"
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
                "text-7xl font-black text-base-content mb-12 text-center drop-shadow-2xl"
            ),
            slide.content
        )),
        "flex flex-col items-center justify-center flex-1 p-16 max-w-6xl mx-auto"
    );
}

Element* render_component() {
    Slide slide = get_slide(current_slide);
    
    return class(
        element("div", children(
            slide_content(slide),
            slide_navigation()
        )),
        arena_sprintf(&r_arena, "h-full w-full flex flex-col transition-all duration-500 ease-in-out %s", slide.background_class)
    );
}
