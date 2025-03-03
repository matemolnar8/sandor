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
    char* data;
    size_t length;
} String;

__attribute__((export_name("render_component")))
String* render_component()
{
    Arena arena;
    String* result = arena_alloc(&arena, sizeof(String));
    if (result == NULL) {
        return NULL;
    }

    const char* message = "my name jeff";
    size_t message_length = 13;

    result->data = arena_alloc(&arena, message_length + 1);

    for (size_t i = 0; i < message_length; ++i) {
        result->data[i] = message[i];
    }
    result->data[message_length] = '\0';
    result->length = message_length;

    printf("render_component: %s\n", result->data);

    return result;
}
