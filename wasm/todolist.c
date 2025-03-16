#include <stdbool.h>
#include "wasm-component.h"

typedef struct {
    char* text;
    bool completed;
} Todo;

typedef struct {
    size_t count;
    size_t capacity;
    Todo** items;
} Todos;

Todos todos = {0};
Arena todo_list_arena = {0};

void add_todo(void* args)
{
    ASSERT(args == NULL);

    Todo* todo = arena_alloc(&todo_list_arena, sizeof(Todo));
    todo->text = arena_sprintf(&todo_list_arena, "Todo #%d", todos.count + 1);
    arena_da_append(&todo_list_arena, &todos, todo);
}

typedef struct {
    int index;
} ToggleTodoArgs;

void toggle_todo(void* args) {
    ASSERT(args != NULL);

    ToggleTodoArgs* toggle_todo_args = (ToggleTodoArgs*)args;
    todos.items[toggle_todo_args->index]->completed = !todos.items[toggle_todo_args->index]->completed;
}

Element* todo_list() {
    Element* todo_list = element("ul", children_empty());

    for (size_t i = 0; i < todos.count; i++) {
        arena_da_append(&r_arena, todo_list->children, text_element(
            "li", 
            arena_sprintf(&r_arena, "%s: %s", todos.items[i]->text, todos.items[i]->completed ? "✅" : "❌")
        ));

        ToggleTodoArgs* toggle_todo_args = arena_alloc(&r_arena, sizeof(ToggleTodoArgs));
        toggle_todo_args->index = i;
        arena_da_append(&r_arena, todo_list->children, button("Toggle", toggle_todo, toggle_todo_args));
    }

    return todo_list;
}

Element* render_component()
{
    return element("div", children(
        text_element("h1", "To-Do list"),
        button("Add todo", add_todo, NULL),
        todo_list()
    ));
}