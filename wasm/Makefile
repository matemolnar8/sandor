hello.wasm: hello.c
	clang                                                                  	      \
		-Wall \
		-Werror \
		-Os \
		-DCLAY_WASM \
		-mbulk-memory \
		--target=wasm32 \
		-nostdlib \
		-Wl,--strip-all \
		-Wl,--export-dynamic \
		-Wl,--no-entry \
		-Wl,--export=__heap_base \
		-std=c17 \
		-Wl,--initial-memory=10485760 \
		-Wl,--allow-undefined \
		-o ../public/hello.wasm \
		hello.c

.PHONY: watch
watch:
	fswatch -o . | xargs -n1 -I{} make hello.wasm