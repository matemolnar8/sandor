import { ClayCanvasRenderer } from "./clay-canvas-renderer";
import { assertAndGet } from "./util/assert-value";

class HelloWorldElement extends HTMLElement {
  clayCanvasRenderer: ClayCanvasRenderer | undefined;
  canvas: HTMLCanvasElement | undefined;
  ctx: CanvasRenderingContext2D | undefined;

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
        canvas {
          width: 100%;
          height: 100%;
        }
      </style>
      <canvas></canvas>
    `;
  }

  get shadowRoot() {
    return assertAndGet(super.shadowRoot, "Could not find shadow root");
  }

  async connectedCallback() {
    this.canvas = assertAndGet(this.shadowRoot.querySelector("canvas"), "Could not find canvas");
    this.ctx = assertAndGet(this.canvas.getContext("2d"), "Could not get canvas context");
    this.clayCanvasRenderer = new ClayCanvasRenderer("./hello.wasm", this.canvas, this.ctx);
    await this.clayCanvasRenderer.init();

    this.updateCanvasSize();
    window.addEventListener("resize", this.updateCanvasSize.bind(this));

    this.clayCanvasRenderer.renderLoop();
  }

  updateCanvasSize() {
    const canvas = assertAndGet(this.canvas, "Canvas not found");
    canvas.width = canvas.clientWidth;
    canvas.height = canvas.clientHeight;
  }
}

customElements.define("hello-world", HelloWorldElement);
