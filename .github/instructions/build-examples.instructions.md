---
applyTo: 'example/**/*.c'
---
After modifying the application C code, you can build the project using the following command in the example directory:

```bash
./nob
```
This will compile the C code and generate the necessary files for the Sandor UI library.
You should use the `playwright` MCP server to run the application for testing. Make sure to have the MCP server running before you start the application.

The dev server needs to be started in the `example` directory:

```bash
cd example
npm run dev
```