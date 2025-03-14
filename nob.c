#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#define NOB_EXPERIMENTAL_DELETE_OLD
#include "nob.h"

#define WASM_CFLAGS "-Wall", "-Werror", "-Os", "-DCLAY_WASM", "-mbulk-memory", "--target=wasm32", "-nostdlib"
#define WASM_LDFLAGS "-Wl,--strip-all", "-Wl,--export-dynamic", "-Wl,--no-entry", "-Wl,--export=__heap_base", \
                     "-std=c17", "-Wl,--initial-memory=10485760", "-Wl,--allow-undefined"

#define SOURCE_FILE "wasm/test.c"

bool build_wasm(void)
{
    nob_log(INFO, "Building test.wasm...");
    
    if (!mkdir_if_not_exists("public")) {
        nob_log(ERROR, "Could not create directory public");
        return false;
    }
    
    Cmd cmd = {0};
    cmd_append(&cmd, "clang");
    cmd_append(&cmd, WASM_CFLAGS);
    cmd_append(&cmd, WASM_LDFLAGS);
    cmd_append(&cmd, "-o", "public/test.wasm");
    cmd_append(&cmd, SOURCE_FILE);
    
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
    
    if (!build_wasm()) {
        return 1;
    }
    
    return 0;
}
