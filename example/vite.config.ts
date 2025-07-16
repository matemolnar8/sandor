import { defineConfig } from "vite";
import tailwindcss from "@tailwindcss/vite";
import { run } from "vite-plugin-run";

export default defineConfig({
  base: process.env.NODE_ENV === "production" ? "/sandor/" : "/",
  plugins: [
    tailwindcss(),
    // nob should be run manually before the build in production
    ...(process.env.NODE_ENV === "production"
      ? []
      : [
          run({
            input: [
              {
                name: "nob",
                run: ["./nob"],
                pattern: ["sandor-apps/*.c", "sandor-apps/*.h"],
              },
            ],
            silent: false,
          }),
        ]),
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
