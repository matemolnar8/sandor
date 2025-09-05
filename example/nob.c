#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#define NOB_EXPERIMENTAL_DELETE_OLD
#include "nob.h"

// Sync with .clangd
#define WASM_CFLAGS "-I..", "-I../thirdparty", "-std=c23", "-Wall", "-Werror", "-Os", "-g", \
                    "-mbulk-memory", "--target=wasm32", "-nostdlib", "-fno-builtin",

#define WASM_LDFLAGS "-Wl,--export-dynamic", "-Wl,--no-entry", "-Wl,--export=__heap_base", \
                     "-Wl,--initial-memory=10485760", "-Wl,--allow-undefined"

#define PUBLIC_DIR "public"

Cmd cmd = { 0 };
Procs procs = { 0 };

bool build_sandor_app(char* name)
{
    char* output_path = temp_sprintf("public/%s.wasm", name);
    char* input_path = temp_sprintf("sandor-apps/%s.c", name);

    const char* input_paths[] = { input_path, "../sandor.h" };

    if (!needs_rebuild(output_path, input_paths, 2)) {
        nob_log(INFO, "%s is up to date", name);
        return true;
    }

    nob_log(INFO, "Building %s...", name);

    cmd_append(&cmd, "clang");
    cmd_append(&cmd, WASM_CFLAGS);
    cmd_append(&cmd, WASM_LDFLAGS);
    cmd_append(&cmd, "-o", output_path);
    cmd_append(&cmd, input_path);

    if (!cmd_run(&cmd, .async = &procs)) {
        nob_log(ERROR, "Failed to start building %s", output_path);
        return false;
    }

    return true;
}

int main(int argc, char** argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    if (!mkdir_if_not_exists(PUBLIC_DIR)) {
        nob_log(ERROR, "Could not create directory public");
        return 1;
    }

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
