#include <stdbool.h>
#include "sandor.h"

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

bool has_error = false;
char input_text[INPUT_BUFFER_CAPACITY] = "\0";
void on_change(const char* text) {
    copy(text, input_text, INPUT_BUFFER_CAPACITY);

    if (input_text[0] == '\0') {
        has_error = true;
    } else {
        has_error = false;
    }
    platform_rerender();
}

void add_todo(void* args)
{
    ASSERT(args == NULL);

    if(input_text[0] == '\0') {
        has_error = true;
        return;
    }

    Todo* todo = arena_alloc(&todo_list_arena, sizeof(Todo));
    *todo = (Todo) {
        .text = arena_strdup(&todo_list_arena, input_text),
        .completed = false
    };

    *input_text = '\0';
    
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
    Element* input_element = attributes(
        input("Enter a new todo", on_change),
        "class", has_error ? "input input-error" : "input"
    );

    return class(
        element("div", children(
            class(text_element("h1", "To-Do list"), "text-3xl font-bold"),
            input_element,
            class(button("Add todo", add_todo, NULL), "btn btn-primary"),
            todo_list()
        )),
        "flex flex-col items-center p-6 gap-4 h-full"
    );
}