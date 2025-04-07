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
    *todo = (Todo) {
        .text = arena_sprintf(&todo_list_arena, "Todo #%d", todos.count + 1),
        .completed = false
    };
    
    arena_da_append(&todo_list_arena, &todos, todo);
}

typedef struct {
    size_t index;
} ToggleTodoArgs;

void toggle_todo(void* args) {
    ASSERT(args != NULL);

    ToggleTodoArgs* toggle_todo_args = (ToggleTodoArgs*)args;
    todos.items[toggle_todo_args->index]->completed = !todos.items[toggle_todo_args->index]->completed;
}

Element* todo_list() {
    Element* todo_list = element("ul", children_empty());

    for (size_t i = 0; i < todos.count; i++) {
        ToggleTodoArgs* toggle_todo_args = arena_alloc(&r_arena, sizeof(ToggleTodoArgs));
        toggle_todo_args->index = i;

        Todo* todo = todos.items[i];
        Element* li_item = element("li", children(
            text_element("span", arena_sprintf(&r_arena, "%s: %s", todo->text, todo->completed ? "✅" : "❌")),
            class(button("Toggle", toggle_todo, toggle_todo_args), "btn ml-2")
        ));
        add_children(todo_list, li_item);
    }

    return todo_list;
}

Element* render_component()
{
    return class(
        element("div", children(
            class(text_element("h1", "To-Do list"), "text-3xl font-bold"),
            class(button("Add todo", add_todo, NULL), "btn btn-primary"),
            todo_list()
        )),
        "flex flex-col items-center p-6 gap-4 h-full"
    );
}