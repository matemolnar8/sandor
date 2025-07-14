# Sandor - C WebAssembly UI Library

Sandor is a recreational UI library for building web UIs in C that compile to WebAssembly. Think React but in C with WASM-to-JS bridge.

## Architecture Overview

- **Core Library**: `sandor.h` - Single-header C library providing DOM-like element creation with arena allocation
- **WASM Bridge**: TypeScript `WasmComponent` class handles C↔JS interop via exported functions and memory manipulation
- **Build System**: `nob` (nob.h-based) builds C apps to WASM, Vite handles TypeScript/CSS
- **Example Apps**: Three demo apps (`test.c`, `todolist.c`) showing different patterns

## Key Development Patterns

### C Application Structure
```c
#include "sandor.h"

// Global state (persistent across renders)
static int count = 0;

// Event callbacks take void* args, rerender manually
void button_callback(void* args) {
    count++;
    // No explicit rerender - platform_rerender() called automatically
}

// Required: Main render function returning root element
Element* render_component() {
    return class(
        element("div", children(
            text_element("h1", "Hello"),
            button("Click me", button_callback, NULL)
        )),
        "flex flex-col gap-4"  // Tailwind classes
    );
}
```

### Element API Patterns
- `element(tag, children)` - Generic HTML elements
- `button(text, callback, args)` - Buttons with click handlers
- `input(placeholder, on_change)` - Text inputs with change handlers
- `text_element(tag, text)` - Elements with static text
- `class(element, "css-classes")` - Apply CSS classes
- `attributes(element, "key", "value", ...)` - Set HTML attributes

### Memory Management
- Uses arena allocators (`r_arena` for render-scoped, custom arenas for persistent data)
- `arena_sprintf(&r_arena, "Count: %d", count)` for dynamic strings
- `arena_reset(&r_arena)` happens automatically each render
- Persistent state requires separate arena (see `todo_list_arena` in todolist.c)

### TypeScript Integration
- `WasmComponent` class loads WASM and manages DOM
- Memory layout introspection via `get_element_layout()` export
- Event handling via `invoke_on_click(index)` and `invoke_on_change(index, value)` exports
- Uses morphdom for efficient DOM diffing

## Development Workflow

### Building
```bash
cd example
npm run dev          # Start dev server with hot reload
./nob               # Manual WASM rebuild
npm run build       # Production build
```

### Adding New Apps
1. Create `example/sandor-apps/myapp.c` with `render_component()` function
2. Add to `nob.c` build list: `build_sandor_app("myapp")`
3. Add to `app-switcher.ts` apps array
4. WASM auto-rebuilds on C file changes via Vite plugin

### Key Build Details
- C flags: `-std=c23 -Wall -Werror -Os --target=wasm32 -nostdlib`
- WASM exports: `render_component`, `invoke_on_click`, `invoke_on_change`, `get_input_buffer`, `get_element_layout`
- Memory setup: 10MB initial memory, bulk memory operations enabled
- Uses `clang` with specific WASM linker flags (see `nob.c` WASM_LDFLAGS)

## Styling with daisyUI + Tailwind
- Uses daisyUI 5 components via CSS classes: `"btn btn-primary"`, `"input input-error"`
- Tailwind for layout: `"flex flex-col gap-4"`, `"w-full h-full"`
- Configure in `style.css`: `@import 'tailwindcss'; @plugin "daisyui";`

## Common Patterns
- **State management**: Global variables in C, persistent across renders
- **Dynamic content**: Use `arena_sprintf()` for formatted strings
- **Lists**: Dynamic arrays with `arena_da_append()`, iterate in render
- **Error handling**: Global flags + conditional styling (`has_error ? "input-error" : ""`)
- **Input handling**: Copy to global buffer, validate in callback, call `platform_rerender()`

## Debugging
- Set `window.sandor = { debug: true }` for debug rerender button
- C `printf()` outputs to browser console via `platform_write`
- WASM memory inspection via browser DevTools
- Vite auto-reloads on WASM changes
