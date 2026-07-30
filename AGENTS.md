# AGENTS.md

## Cursor Cloud specific instructions

Sandor is a C→WASM UI library (`sandor.h`) with a Vite/TypeScript demo in `example/`. C sources in `example/sandor-apps/*.c` are compiled to `.wasm` by the `nob` build system and rendered by the TS host.

### Toolchain notes
- The WASM build requires clang with the `wasm32` target and the `wasm-ld` linker (from the `lld` apt package). Without `lld`, `./nob` fails with `Executable "wasm-ld-18" doesn't exist!`. It is provided by the VM snapshot; it is a system package, not an npm dependency, so it is not in the update script.
- `nob` is a compiled C build tool. Bootstrap it once with `npm run nob:bootstrap` (`cc -o nob nob.c`). The `nob` binary and `public/*.wasm` are gitignored, so after a fresh clone you must bootstrap and build before/at dev startup. `nob` auto-rebuilds itself when `nob.c` changes.

### Running (from `example/`)
- Dev: `npm run dev` runs Vite (port 5173) plus `nob` in watch mode; editing `sandor-apps/*.c|*.h` or `sandor.h` rebuilds the affected `.wasm`, and Vite full-reloads on `.wasm` changes. If the `nob` binary is missing, run `npm run nob:bootstrap` first.
- Build/typecheck: `npm run build` runs `tsc` then `vite build` (set `NODE_ENV=production`).
- Standalone WASM rebuild: `./nob` from `example/`.

### Adding an app
New `example/sandor-apps/<name>.c`, add a `build_sandor_app("<name>")` call in `nob.c`, and list it in `example/src/app-switcher.ts`.
