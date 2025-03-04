#include <stddef.h>
#define ARENA_IMPLEMENTATION
#define ARENA_NOSTDIO
#define ARENA_ASSERT(cond) (!(cond) ? printf("%s:%d: %s: Assertion `%s' failed.", __FILE__, __LINE__, __func__, #cond), __builtin_trap() : 0)
#include <stdarg.h>
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
#define ARENA_BACKEND ARENA_BACKEND_WASM_HEAPBASE
#include "arena.h"

typedef struct {
    size_t length;
    char* data;
} String;

typedef struct {
    String* type;
    String* text;
} Element;

Arena render_result_arena = {0};

int render_count = 0;

__attribute__((export_name("render_component")))
Element* render_component()
{
    arena_reset(&render_result_arena);
    render_count++;

    Element* result = arena_alloc(&render_result_arena, sizeof(Element));

    char* message = arena_alloc(&render_result_arena, WRITE_BUFFER_CAPACITY);
    int message_length = stbsp_sprintf(message, "Render count: %d", render_count);

    result->text = arena_alloc(&render_result_arena, sizeof(String));
    result->text->length = message_length;
    result->text->data = message;

    result->type = arena_alloc(&render_result_arena, sizeof(String));
    result->type->length = 2;
    result->type->data = "h1";

    return result;
}
