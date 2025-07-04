const apps = [
  { value: "canvas", label: "Canvas" },
  { value: "test", label: "Test" },
  { value: "todolist", label: "Todolist" },
];

function createApp(name: string) {
  const app = document.createElement("wasm-shell-component");
  app.setAttribute("name", name);
  app.className = "w-full h-full";
  return app;
}

function switchApp(name: string) {
  const container = document.getElementById("app-container")!;
  container.innerHTML = "";
  container.appendChild(createApp(name));
}

document.addEventListener("DOMContentLoaded", () => {
  const select = document.getElementById("app-selector") as HTMLSelectElement;

  apps.forEach((app, i) => {
    const option = document.createElement("option");
    option.value = app.value;
    option.textContent = app.label;
    if (i === 0) option.selected = true;
    select.appendChild(option);
  });

  select.addEventListener("change", (e) => {
    switchApp((e.target as HTMLSelectElement).value);
  });

  switchApp(apps[0].value);
});
