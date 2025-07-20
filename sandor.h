// Copyright 2025 Máté Molnár <doglotteger@gmail.com>

// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:

// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef SANDOR_H
#define SANDOR_H

#include <stddef.h>
#include <stdarg.h>
#include <stddef.h>
#include "macros.h"
#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"
#define OLIVEC_IMPLEMENTATION
#include "olive.c"

void platform_write(void *buffer, size_t len);

#define WRITE_BUFFER_CAPACITY 4096
char write_buffer[WRITE_BUFFER_CAPACITY];
int printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int n = stbsp_vsnprintf(write_buffer, WRITE_BUFFER_CAPACITY, fmt, args);
    va_end(args);
    platform_write(write_buffer, n);
    return n;
}

#define ASSERT(cond) (!(cond) ? printf("%s:%d: %s: Assertion `%s' failed.", __FILE__, __LINE__, __func__, #cond), __builtin_trap() : 0)

#define ARENA_ASSERT(cond) ASSERT(cond)
#define ARENA_IMPLEMENTATION
#define ARENA_NOSTDIO
#define ARENA_BACKEND ARENA_BACKEND_WASM_HEAPBASE
#include "arena.h"

void copy(const char *src, char *dst, size_t max_len) {
    size_t i = 0;

    while (src[i] != '\0') {
        ASSERT(i < max_len);
        dst[i] = src[i];  
        i++;
    }

    dst[i] = '\0';
}

typedef struct Element Element;

typedef struct {
    size_t count;
    size_t capacity;
    Element** items;
} Children;

typedef struct {
    const char* name;
    const char* value;
} Attribute;

typedef struct {
    size_t count;
    size_t capacity;
    Attribute** items;
} Attributes;

typedef enum {
    ELEMENT_GENERIC = 0,
    ELEMENT_BUTTON = 1,
    ELEMENT_INPUT = 2,
    ELEMENT_CANVAS = 3,
} ElementType;

typedef struct {
    const char* tag;
} GenericElement;

typedef struct {
    void (*on_click)(void*);
    void* on_click_args;
} ButtonElement;

typedef struct {
    char* placeholder;
    void (*on_change)(const char*);
} InputElement;

typedef struct {
    char* id;
    size_t width;
    size_t height;
} CanvasElement;

struct Element {
    ElementType type;
    size_t index;
    
    // Common properties for all elements
    char* text;
    Children* children;
    Attributes* attributes;
    
    // Type-specific data
    union {
        GenericElement generic;
        ButtonElement button;
        InputElement input;
        CanvasElement canvas;
    };
};

typedef struct {
    size_t count;
    size_t capacity;
    Element** items;
} Elements;

#define struct_wrapper(type, ...) (type) __VA_ARGS__
#define _init_struct(type) \
    void *_init_##type(type value) { \
        type *result = arena_alloc(&r_arena, sizeof(type)); \
        *result = value; \
        return result; \
    } 

void platform_rerender();
void platform_draw_canvas(char* canvas_id, Olivec_Canvas* canvas);
void platform_on_animation_frame(void (*callback)(float dt));
void platform_clear_animation_frame(void (*callback)(float dt));

Arena r_arena = {0};
Elements r_elements = {0};

_init_struct(Element);
_init_struct(Attribute);
_init_struct(Children);
_init_struct(Attributes);

#define ELEMENT(...) _init_Element(struct_wrapper(Element, __VA_ARGS__))
#define ATTRIBUTE(...) _init_Attribute(struct_wrapper(Attribute, __VA_ARGS__))
#define CHILDREN(...) _init_Children(struct_wrapper(Children, __VA_ARGS__))
#define ATTRIBUTES(...) _init_Attributes(struct_wrapper(Attributes, __VA_ARGS__))

// Adapted from arena.h using stb_sprintf
char *arena_sprintf(Arena* a, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int n = stbsp_vsnprintf(NULL, 0, format, args);
    va_end(args);

    ARENA_ASSERT(n >= 0);
    char *result = (char*)arena_alloc(a, n + 1);
    va_start(args, format);
    stbsp_vsnprintf(result, n + 1, format, args);
    va_end(args);

    return result;
}

#define children(...) _children(_NARG(__VA_ARGS__), __VA_ARGS__)
#define children_empty() _children(0)

Children* _children(size_t count, ...) {
    Children* result = CHILDREN({
        .count = 0,
        .capacity = 0,
        .items = NULL
    });

    va_list args;
    va_start(args, count);
    for (size_t i = 0; i < count; i++) {
        Element* element = va_arg(args, Element*);
        arena_da_append(&r_arena, result, element);
    }
    va_end(args);

    return result;
}

#define add_children(parent, ...) _add_children(parent, _NARG(__VA_ARGS__), __VA_ARGS__)
void _add_children(Element* parent, size_t count, ...) {
    va_list args;
    va_start(args, count);
    for (size_t i = 0; i < count; i++) {
        Element* element = va_arg(args, Element*);
        arena_da_append(&r_arena, parent->children, element);
    }
    va_end(args);
};

#define attributes(element, ...) _attributes(element, _NARG(__VA_ARGS__), __VA_ARGS__)
#define class(element, value) _attributes(element, 2, "class", value)
Element* _attributes(Element* element, size_t count, ...) {
    ASSERT(count % 2 == 0);

    element->attributes = ATTRIBUTES({
        .count = 0,
        .capacity = 0,
        .items = NULL
    });

    va_list args;
    va_start(args, count);
    for (size_t i = 0; i < count / 2; i++) {
        const char* name = va_arg(args, const char*);
        const char* value = va_arg(args, const char*);
        
        Attribute* attribute = ATTRIBUTE({
            .name = arena_strdup(&r_arena, name),
            .value = arena_strdup(&r_arena, value)
        });
        arena_da_append(&r_arena, element->attributes, attribute);
    }
    va_end(args);

    return element;
}

Element* element(const char* tag, Children* children)
{
    Element* result = ELEMENT({
        .type = ELEMENT_GENERIC,
        .index = r_elements.count,
        .text = NULL,
        .children = children,
        .attributes = NULL,
        .generic = {
            .tag = arena_strdup(&r_arena, tag)
        }
    });

    arena_da_append(&r_arena, &r_elements, result);

    return result;
}

Element* button(char* text, void (*callback)(void*), void* args)
{
    Element* result = ELEMENT({
        .type = ELEMENT_BUTTON,
        .index = r_elements.count,
        .text = text,
        .children = children_empty(),
        .attributes = NULL,
        .button = {
            .on_click = callback,
            .on_click_args = args
        }
    });

    arena_da_append(&r_arena, &r_elements, result);

    return result;
}

Element* input(const char* placeholder, void (*on_change)(const char*))
{
    Element* result = ELEMENT({
        .type = ELEMENT_INPUT,
        .index = r_elements.count,
        .text = NULL,
        .children = NULL,
        .attributes = NULL,
        .input = {
            .placeholder = arena_strdup(&r_arena, placeholder),
            .on_change = on_change
        }
    });

    arena_da_append(&r_arena, &r_elements, result);

    return result;
}

Element* canvas(char* id, size_t width, size_t height)
{
    Element* result = ELEMENT({
        .type = ELEMENT_CANVAS,
        .index = r_elements.count,
        .text = NULL,
        .children = NULL,
        .attributes = NULL,
        .canvas = {
            .width = width,
            .height = height,
            .id = id ? arena_strdup(&r_arena, id) : arena_sprintf(&r_arena, "canvas-%zu", r_elements.count)
        }
    });

    arena_da_append(&r_arena, &r_elements, result);

    return result;
}

#define TEXT_CAPACITY 256

Element* text_element_empty(const char* tag)
{
    Element* result = element(tag, NULL);
    result->text = arena_alloc(&r_arena, TEXT_CAPACITY);
    return result;
}

Element* text_element(const char* tag, const char* text)
{
    Element* result = element(tag, NULL);
    result->text = arena_strdup(&r_arena, text);
    return result;
}

Element* render_component();

[[clang::export_name("init_component")]]
void init_component();

[[clang::export_name("render_component")]]
Element* render_component_internal() {
    arena_reset(&r_arena);
    r_elements.count = 0;
    r_elements.capacity = 0;

    return render_component();
}

[[clang::export_name("invoke_on_click")]]
void invoke_on_click(size_t element_index) {
    ASSERT(r_elements.count > 0 && element_index < r_elements.count);
    Element* element = r_elements.items[element_index];

    if (element->type == ELEMENT_BUTTON && element->button.on_click) {
        element->button.on_click(element->button.on_click_args);
        platform_rerender();
    }
}

[[clang::export_name("invoke_on_change")]]
void invoke_on_change(size_t element_index, const char* value) {
    ASSERT(r_elements.count > 0 && element_index < r_elements.count);
    Element* element = r_elements.items[element_index];

    if (element->type == ELEMENT_INPUT && element->input.on_change) {
        element->input.on_change(value);
        platform_rerender();
    }
}

[[clang::export_name("invoke_animation_frame_callback")]]
void invoke_animation_frame_callback(void (*callback)(float dt), float dt) {
    ASSERT(callback != NULL);
    callback(dt);
}

Arena input_arena = {0};
#define INPUT_BUFFER_CAPACITY 4096
[[clang::export_name("get_input_buffer")]]
void* get_input_buffer() {
    return arena_alloc(&input_arena, INPUT_BUFFER_CAPACITY);
}

// Export Element struct layout as a packed array of offsets
[[clang::export_name("get_element_layout")]]
const size_t* get_element_layout() {
    static const size_t layout[] = {
        offsetof(Element, type),        // 0: element type (enum)
        offsetof(Element, index),       // 1: element index
        offsetof(Element, text),        // 2: common text field
        offsetof(Element, children),    // 3: common children field
        offsetof(Element, attributes),  // 4: common attributes field
        
        // Union offset
        offsetof(Element, generic),     // 5: union offset
        
        // Generic element offsets (relative to union start)
        offsetof(GenericElement, tag),  // 6
        
        // Button element offsets (relative to union start)
        offsetof(ButtonElement, on_click),     // 7
        offsetof(ButtonElement, on_click_args), // 8
        
        // Input element offsets (relative to union start)
        offsetof(InputElement, placeholder),   // 9
        offsetof(InputElement, on_change),     // 10

        // Canvas element offsets (relative to union start)
        offsetof(CanvasElement, id),           // 11
        offsetof(CanvasElement, width),        // 12
        offsetof(CanvasElement, height),       // 13

        // Total size of the Element struct
        sizeof(Element)
    };
    return layout;
}

#endif // SANDOR_H