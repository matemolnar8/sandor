#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#define NOB_EXPERIMENTAL_DELETE_OLD
#include "nob.h"

// Sync with .clangd
#define WASM_CFLAGS "-I..", "-I../thirdparty", "-Ibuild", "-std=c23", "-Wall", "-Werror", "-Os", "-g", \
                    "-mbulk-memory", "--target=wasm32", "-nostdlib", "-fno-builtin",

#define WASM_LDFLAGS "-Wl,--export-dynamic", "-Wl,--no-entry", "-Wl,--export=__heap_base", \
                     "-Wl,--initial-memory=10485760", "-Wl,--allow-undefined"

#define PUBLIC_DIR "public"
#define BUILD_DIR "build"
#define BUILD_TOOLS_DIR "build/tools"
#define BUILD_ASSETS_DIR "build/assets"

Cmd cmd = { 0 };
Procs procs = { 0 };

bool build_png2c(void)
{
    const char *output_path = BUILD_TOOLS_DIR "/png2c";
    const char *input_paths[] = {
        "tools/png2c.c",
        "tools/stb_image.h",
        "nob.h",
    };

    if (!needs_rebuild(output_path, input_paths, NOB_ARRAY_LEN(input_paths))) {
        nob_log(INFO, "png2c is up to date");
        return true;
    }

    nob_log(INFO, "Building png2c...");
    cmd_append(&cmd, "clang", "-Wall", "-Wextra", "-O2",
               "-D_POSIX_C_SOURCE=200809L",
               "-I.", "-Itools",
               "-o", output_path,
               "tools/png2c.c", "-lm");
    if (!cmd_run(&cmd)) {
        nob_log(ERROR, "Failed to build png2c");
        return false;
    }

    return true;
}

bool build_png_asset(const char *name)
{
    char *input_path = temp_sprintf("assets/%s.png", name);
    char *output_path = temp_sprintf(BUILD_ASSETS_DIR "/%s.c", name);

    if (!needs_rebuild1(output_path, input_path)) {
        nob_log(INFO, "asset %s is up to date", name);
        return true;
    }

    nob_log(INFO, "Generating asset %s...", name);
    cmd_append(&cmd, BUILD_TOOLS_DIR "/png2c",
               "-n", name,
               "-o", output_path,
               input_path);
    if (!cmd_run(&cmd)) {
        nob_log(ERROR, "Failed to generate asset %s", name);
        return false;
    }

    return true;
}

bool build_assets(void)
{
    if (!build_png2c()) return false;
    if (!build_png_asset("dvd_logo")) return false;
    return true;
}

bool build_sandor_app(char *name)
{
    char *output_path = temp_sprintf("public/%s.wasm", name);
    char *input_path = temp_sprintf("sandor-apps/%s.c", name);

    const char *input_paths[8];
    size_t input_paths_count = 0;
    input_paths[input_paths_count++] = input_path;
    input_paths[input_paths_count++] = "../sandor.h";

    // DVD app embeds the generated logo sprite.
    if (strcmp(name, "dvd") == 0) {
        input_paths[input_paths_count++] = BUILD_ASSETS_DIR "/dvd_logo.c";
        input_paths[input_paths_count++] = "sandor-apps/dvd.h";
    }

    if (!needs_rebuild(output_path, input_paths, input_paths_count)) {
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

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    if (!mkdir_if_not_exists(PUBLIC_DIR)) {
        nob_log(ERROR, "Could not create directory public");
        return 1;
    }

    if (!mkdir_if_not_exists(BUILD_DIR)) {
        nob_log(ERROR, "Could not create directory build");
        return 1;
    }

    if (!mkdir_if_not_exists(BUILD_TOOLS_DIR)) {
        nob_log(ERROR, "Could not create directory build/tools");
        return 1;
    }

    if (!mkdir_if_not_exists(BUILD_ASSETS_DIR)) {
        nob_log(ERROR, "Could not create directory build/assets");
        return 1;
    }

    if (!build_assets()) {
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

    if (!build_sandor_app("dvd")) {
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
