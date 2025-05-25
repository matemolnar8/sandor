import { defineConfig } from "vite";
import tailwindcss from "@tailwindcss/vite";
import { run } from "vite-plugin-run";

export default defineConfig({
  plugins: [
    tailwindcss(),
    run({
      input: [
        {
          name: "nob",
          run: ["./nob"],
          pattern: ["wasm/**/*.c", "wasm/**/*.h"],
        },
      ],
      silent: false,
    }),
    {
      name: "reload",
      configureServer(server) {
        const { ws, watcher } = server;
        watcher.on("change", (file) => {
          if (file.endsWith(".wasm")) {
            ws.send({
              type: "full-reload",
            });
          }
        });
      },
    },
  ],
});
