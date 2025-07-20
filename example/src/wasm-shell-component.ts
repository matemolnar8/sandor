import { WasmComponent } from "./wasm-component";
import { assertAndGet } from "./util/assert-value";
import stylesheet from "../style.css?inline";

class WasmShellComponent extends HTMLElement {
  wasmComponent: WasmComponent | undefined;

  constructor() {
    super();
    this.attachShadow({ mode: "open" });
    const sheet = new CSSStyleSheet();
    sheet.replaceSync(stylesheet);

    this.shadowRoot?.adoptedStyleSheets.push(sheet);

    this.shadowRoot.innerHTML = `
      <div id="root" class="h-full"></div>
    `;
  }

  get shadowRoot() {
    return assertAndGet(super.shadowRoot, "Could not find shadow root");
  }

  async connectedCallback() {
    const name = this.getAttribute("name") || "test";
    this.wasmComponent = new WasmComponent(`./${name}.wasm`);

    const root = this.shadowRoot.getElementById("root");

    if (!root) {
      return;
    }

    await this.wasmComponent.init(root);

    this.shadowRoot.getElementById("debugRerender")?.addEventListener("click", () => {
      this.wasmComponent?.render();
    });

    this.wasmComponent?.render();
  }

  disconnectedCallback() {
    this.wasmComponent?.destroy();
    this.wasmComponent = undefined;
  }
}

customElements.define("wasm-shell-component", WasmShellComponent);
