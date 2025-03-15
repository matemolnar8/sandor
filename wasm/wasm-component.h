#include <stddef.h>
#include <stdarg.h>
#include "macros.h"
#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

#define WRITE_BUFFER_CAPACITY 4096
char write_buffer[WRITE_BUFFER_CAPACITY];
void platform_write(void *buffer, size_t len);
int printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int n = stbsp_vsnprintf(write_buffer, WRITE_BUFFER_CAPACITY, fmt, args);
    va_end(args);
    platform_write(write_buffer, n);
    return n;
}

#define ARENA_IMPLEMENTATION
#define ARENA_NOSTDIO
#define ARENA_ASSERT(cond) (!(cond) ? printf("%s:%d: %s: Assertion `%s' failed.", __FILE__, __LINE__, __func__, #cond), __builtin_trap() : 0)
#define ARENA_BACKEND ARENA_BACKEND_WASM_HEAPBASE
#include "arena.h"

typedef struct Element Element;

typedef struct {
    size_t count;
    size_t capacity;
    Element** items;
} Children;

struct Element {
    const char* type;
    char* text;
    Children* children;
    void (*on_click)();
};

void platform_rerender();

Arena render_result_arena = {0};

#define children(...) _children(_NARG(__VA_ARGS__), __VA_ARGS__)
#define children_empty() _children(0)

Children* _children(size_t count, ...) {
    Children* result = arena_alloc(&render_result_arena, sizeof(Children));
    result->count = 0;
    result->capacity = 0;
    result->items = NULL;

    va_list args;
    va_start(args, count);
    for (size_t i = 0; i < count; i++) {
        Element* element = va_arg(args, Element*);
        arena_da_append(&render_result_arena, result, element);
    }
    va_end(args);

    return result;
}

Element* element(const char* type, Children* children)
{
    Element* result = arena_alloc(&render_result_arena, sizeof(Element));

    result->type = type;
    result->text = NULL;
    result->children = children;

    return result;
}

Element* button(char* text, void (*callback)())
{
    Element* result = element("button", children_empty());

    result->text = text;
    result->on_click = callback;

    return result;
}

#define TEXT_CAPACITY 256

Element* text_element(const char* type)
{
    Element* result = element(type, NULL);

    result->text = arena_alloc(&render_result_arena, TEXT_CAPACITY);

    return result;
}

Element* text_element_with_text(const char* type, const char* text)
{
    Element* result = element(type, NULL);

    result->text = arena_strdup(&render_result_arena, text);

    return result;
}

Element* render_component();

[[clang::export_name("render_component")]]
Element* render_component_internal() {
    arena_reset(&render_result_arena);

    return render_component();
}

[[clang::export_name("invoke_on_click")]]
void invoke_on_click(Element* element) {
    if (element->on_click) {
        element->on_click();
        platform_rerender();
    }
}