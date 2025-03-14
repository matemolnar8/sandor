import { WasmComponent } from "./wasm-component";
import { assertAndGet } from "./util/assert-value";

class WasmTestComponentElement extends HTMLElement {
  wasmComponent: WasmComponent | undefined;

  constructor() {
    super();
    this.attachShadow({ mode: "open" });

    this.shadowRoot.innerHTML = `
      <div id="test"></div>
      <button id="debugRerender">Debug - Rerender</button>      
    `;
  }

  get shadowRoot() {
    return assertAndGet(super.shadowRoot, "Could not find shadow root");
  }

  async connectedCallback() {
    this.wasmComponent = new WasmComponent("./test.wasm");

    const root = this.shadowRoot.getElementById("test");

    if (!root) {
      return;
    }

    await this.wasmComponent.init(root);

    this.shadowRoot.getElementById("debugRerender")?.addEventListener("click", () => {
      this.wasmComponent?.render();
    });

    this.wasmComponent?.render();
  }
}

customElements.define("wasm-test-component", WasmTestComponentElement);
