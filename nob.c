#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#define NOB_EXPERIMENTAL_DELETE_OLD
#include "nob.h"

#define WASM_CFLAGS  "-std=c23", "-Wall", "-Werror", "-mbulk-memory", "--target=wasm32", "-nostdlib", "-Os"
#define WASM_LDFLAGS "-Wl,--strip-all", "-Wl,--export-dynamic", "-Wl,--no-entry", "-Wl,--export=__heap_base", \
                     "-Wl,--initial-memory=10485760", "-Wl,--allow-undefined"


bool build_wasm_component(char* name)
{
    nob_log(INFO, "Building %s...", name);
    
    if (!mkdir_if_not_exists("public")) {
        nob_log(ERROR, "Could not create directory public");
        return false;
    }
    
    Cmd cmd = {0};
    cmd_append(&cmd, "clang");
    cmd_append(&cmd, WASM_CFLAGS);
    cmd_append(&cmd, WASM_LDFLAGS);
    cmd_append(&cmd, "-o", temp_sprintf("public/%s.wasm"));
    cmd_append(&cmd, temp_sprintf("wasm/%s.c", name));
    
    if (!cmd_run_sync(cmd)) {
        nob_log(ERROR, "Failed to build test.wasm");
        return false;
    }
    
    nob_log(INFO, "Successfully built public/test.wasm");
    return true;
}


int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    
    if (!build_wasm_component("test")) {
        return 1;
    }

    if (!build_wasm_component("todolist")) {
        return 1;
    }
    
    return 0;
}
