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

void add_todo()
{
    Todo* todo = arena_alloc(&todo_list_arena, sizeof(Todo));
    todo->text = arena_sprintf(&todo_list_arena, "Todo #%d", todos.count + 1);
    arena_da_append(&todo_list_arena, &todos, todo);
    platform_rerender();
}

void toggle_todo() {
    // TODO: use index from button click event
    todos.items[0]->completed = !todos.items[0]->completed;
    platform_rerender();
}

Element* todo_list() {
    Element* todo_list = element("ul", children_empty());

    for (size_t i = 0; i < todos.count; i++) {
        Element* todo_element = text_element("li");
        todo_element->text = arena_sprintf(&render_result_arena, "%s: %s", todos.items[i]->text, todos.items[i]->completed ? "✅" : "❌");
        arena_da_append(&render_result_arena, todo_list->children, todo_element);
        arena_da_append(&render_result_arena, todo_list->children, button("Toggle", toggle_todo));
    }

    return todo_list;
}

Element* render_component()
{
    return element("div", children(
        text_element_with_text("h1", "To-Do list"),
        button("Add todo", add_todo),
        todo_list()
    ));
}