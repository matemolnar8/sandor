#include <stddef.h>

#define MEMORY_POOL_SIZE 8 * 1024 * 1024 // 8 MB

static char memory_pool[MEMORY_POOL_SIZE];
static size_t memory_offset = 0;

void *malloc(size_t size)
{
    if (memory_offset + size > MEMORY_POOL_SIZE)
    {
        return NULL; // Out of memory
    }

    void *ptr = &memory_pool[memory_offset];
    memory_offset += size;
    return ptr;
}

void *memcpy(void *dest, const void *src, size_t n)
{
    char *d = dest;
    const char *s = src;
    while (n--)
    {
        *d++ = *s++;
    }
    return dest;
}

void *memset(void *s, int c, size_t n)
{
    unsigned char *p = s;
    while (n--)
    {
        *p++ = (unsigned char)c;
    }
    return s;
}