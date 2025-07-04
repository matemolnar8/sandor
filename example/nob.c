#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#define NOB_EXPERIMENTAL_DELETE_OLD
#include "nob.h"

#define WASM_CFLAGS  "-I..", "-I../lib", "-std=c23", "-Wall", "-Werror", "-Os", "-g", \
                     "-mbulk-memory", "--target=wasm32", "-nostdlib"

#define WASM_LDFLAGS "-Wl,--export-dynamic", "-Wl,--no-entry", "-Wl,--export=__heap_base", \
                     "-Wl,--initial-memory=10485760", "-Wl,--allow-undefined"


bool build_sandor_app(char* name)
{
    nob_log(INFO, "Building %s...", name);
    
    if (!mkdir_if_not_exists("public")) {
        nob_log(ERROR, "Could not create directory public");
        return false;
    }
    
    char* output_path = temp_sprintf("public/%s.wasm", name);

    Cmd cmd = {0};
    cmd_append(&cmd, "clang");
    cmd_append(&cmd, WASM_CFLAGS);
    cmd_append(&cmd, WASM_LDFLAGS);
    cmd_append(&cmd, "-o", output_path);
    cmd_append(&cmd, temp_sprintf("sandor-apps/%s.c", name));
    
    if (!cmd_run_sync(cmd)) {
        nob_log(ERROR, "Failed to build %s", output_path);
        return false;
    }
    
    nob_log(INFO, "Successfully built %s", output_path);
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
    
    return 0;
}
