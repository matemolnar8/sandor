#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#define NOB_EXPERIMENTAL_DELETE_OLD
#include "nob.h"
#include <stdbool.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#ifdef __APPLE__
#include <sys/event.h>
#elif defined(__linux__)
#include <sys/inotify.h>
#include <limits.h>
#endif

#define WASM_CFLAGS "-Wall", "-Werror", "-Os", "-DCLAY_WASM", "-mbulk-memory", "--target=wasm32", "-nostdlib"
#define WASM_LDFLAGS "-Wl,--strip-all", "-Wl,--export-dynamic", "-Wl,--no-entry", "-Wl,--export=__heap_base", \
                     "-std=c17", "-Wl,--initial-memory=10485760", "-Wl,--allow-undefined"

#define SOURCE_FILE "wasm/hello.c"

bool build_wasm(void)
{
    nob_log(INFO, "Building hello.wasm...");
    
    if (!mkdir_if_not_exists("public")) {
        nob_log(ERROR, "Could not create directory public");
        return false;
    }
    
    Cmd cmd = {0};
    cmd_append(&cmd, "clang");
    cmd_append(&cmd, WASM_CFLAGS);
    cmd_append(&cmd, WASM_LDFLAGS);
    cmd_append(&cmd, "-o", "public/hello.wasm");
    cmd_append(&cmd, SOURCE_FILE);
    
    if (!cmd_run_sync(cmd)) {
        nob_log(ERROR, "Failed to build hello.wasm");
        return false;
    }
    
    nob_log(INFO, "Successfully built public/hello.wasm");
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
