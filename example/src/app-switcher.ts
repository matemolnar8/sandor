const apps = [
  { value: "presentation", label: "Presentation" },
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

function getInitialApp(): string {
  const urlParams = new URLSearchParams(window.location.search);
  const appParam = urlParams.get('app');
  
  // Check if the app parameter is valid
  if (appParam && apps.some(app => app.value === appParam)) {
    return appParam;
  }
  
  // Default to first app
  return apps[0].value;
}

function switchApp(name: string) {
  const container = document.getElementById("app-container")!;
  container.innerHTML = "";
  container.appendChild(createApp(name));
  
  // Update the button label
  const label = document.getElementById("app-selector-label")!;
  const selectedApp = apps.find(app => app.value === name);
  label.textContent = selectedApp?.label || name;
}

document.addEventListener("DOMContentLoaded", () => {
  const dropdown = document.getElementById("app-selector-dropdown") as HTMLUListElement;

  // Populate dropdown items
  apps.forEach((app) => {
    const li = document.createElement("li");
    const button = document.createElement("button");
    button.className = "btn btn-sm btn-block btn-ghost justify-start";
    button.textContent = app.label;
    button.addEventListener("click", () => {
      switchApp(app.value);
      // Close dropdown by removing focus
      dropdown.blur();
    });
    li.appendChild(button);
    dropdown.appendChild(li);
  });

  // Initialize with app from URL params or first app
  const initialApp = getInitialApp();
  switchApp(initialApp);
});
