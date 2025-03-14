import { WasmComponent, ResultElement } from "./wasm-component";
import { assertAndGet } from "./util/assert-value";

class HelloWorldElement extends HTMLElement {
  wasmComponent: WasmComponent | undefined;

  constructor() {
    super();
    this.attachShadow({ mode: "open" });

    this.shadowRoot.innerHTML = `
      <style>
        :host {
          display: block;
          width: 100%;
          height: 100%;
          background-color: white;
        }
      </style>
      <div id="hello"></div>
      <button id="debugRerender">Render</button>      
    `;
  }

  get shadowRoot() {
    return assertAndGet(super.shadowRoot, "Could not find shadow root");
  }

  async connectedCallback() {
    this.wasmComponent = new WasmComponent("./hello.wasm");
    await this.wasmComponent.init();

    this.render();

    this.shadowRoot.getElementById("debugRerender")?.addEventListener("click", () => {
      this.render();
    });
  }

  render() {
    const parent = this.shadowRoot.getElementById("hello");
    if (parent) {
      parent.innerHTML = "";
      const root = this.wasmComponent!.render();

      const renderElement = (renderResult: ResultElement, parentElement: HTMLElement) => {
        const element = document.createElement(renderResult.type);

        if (renderResult.text) {
          element.textContent = renderResult.text;
        }

        if (Array.isArray(renderResult.children)) {
          renderResult.children.forEach((childResult) => renderElement(childResult, element));
        }

        parentElement.appendChild(element);
      };

      renderElement(root, parent);
    }
  }
}

customElements.define("hello-world", HelloWorldElement);
