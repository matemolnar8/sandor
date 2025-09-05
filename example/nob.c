#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#define NOB_EXPERIMENTAL_DELETE_OLD
#include "nob.h"

// Sync with .clangd
#define WASM_CFLAGS  "-I..", "-I../thirdparty", "-std=c23", "-Wall", "-Werror", "-Os", "-g", \
                     "-mbulk-memory", "--target=wasm32", "-nostdlib", "-fno-builtin", 

// Sync with .clangd
#define WASM_LDFLAGS "-Wl,--export-dynamic", "-Wl,--no-entry", "-Wl,--export=__heap_base", \
                     "-Wl,--initial-memory=10485760", "-Wl,--allow-undefined"


Cmd cmd = {0};
Procs procs = {0};

bool build_sandor_app(char* name)
{
    nob_log(INFO, "Building %s...", name);
    
    if (!mkdir_if_not_exists("public")) {
        nob_log(ERROR, "Could not create directory public");
        return false;
    }
    
    char* output_path = temp_sprintf("public/%s.wasm", name);

    cmd_append(&cmd, "clang");
    cmd_append(&cmd, WASM_CFLAGS);
    cmd_append(&cmd, WASM_LDFLAGS);
    cmd_append(&cmd, "-o", output_path);
    cmd_append(&cmd, temp_sprintf("sandor-apps/%s.c", name));
    
    if (!cmd_run(&cmd, .async = &procs)) {
        nob_log(ERROR, "Failed to start building %s", output_path);
        return false;
    }
    
    return true;
}


int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    
    if (!build_sandor_app("test")) {
        return 1;
    }

    if (!build_sandor_app("todolist")) {
        return 1;
    }

    if (!build_sandor_app("canvas")) {
        return 1;
    }
    
    if (!build_sandor_app("presentation")) {
        return 1;
    }

    if (!procs_flush(&procs)) {
        return 1;
    }
    
    return 0;
}
