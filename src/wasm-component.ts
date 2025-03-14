import { assertAndGet } from "./util/assert-value.js";

type WasmInstance = {
  exports: {
    memory: WebAssembly.Memory;
    render_component: () => number;
    invoke_on_click: (elementPtr: number) => void;
  };
};

const decoder = new TextDecoder();

export type ResultElement = {
  type: string;
  text?: string;
  children?: ResultElement[];
  onClick?: () => void;
};

export class WasmComponent {
  #instance: (WebAssembly.Instance & WasmInstance) | undefined;
  #memoryDataView: DataView | undefined;
  wasmPath: string;
  parent: HTMLElement | undefined;

  constructor(wasmPath: string) {
    this.wasmPath = wasmPath;
  }

  get instance() {
    return assertAndGet(this.#instance, "Instance not found");
  }

  get memoryDataView() {
    return assertAndGet(this.#memoryDataView, "Memory data view not found");
  }

  async init(parent: HTMLElement) {
    this.parent = parent;
    this.#instance = (
      await WebAssembly.instantiateStreaming(fetch(this.wasmPath), {
        env: {
          platform_write: (buf: number, len: number) => {
            const text = decoder.decode(new Uint8Array(this.instance.exports.memory.buffer, buf, len));
            console.log(text);
          },
          platform_rerender: () => {
            this.render();
          },
        },
      })
    ).instance as WasmInstance;

    this.#memoryDataView = new DataView(new Uint8Array(this.instance.exports.memory.buffer).buffer);
  }

  render() {
    const resultAddr = this.instance.exports.render_component();
    const root = this.readElement(resultAddr);

    if (!this.parent) {
      return;
    }
    this.parent.innerHTML = "";

    const renderElement = (renderResult: ResultElement, parentElement: HTMLElement) => {
      const element = document.createElement(renderResult.type);

      if (renderResult.text) {
        element.textContent = renderResult.text;
      }

      if (Array.isArray(renderResult.children)) {
        renderResult.children.forEach((childResult) => renderElement(childResult, element));
      }

      if (renderResult.onClick) {
        element.addEventListener("click", renderResult.onClick);
      }

      parentElement.appendChild(element);
    };

    renderElement(root, this.parent);
  }

  readElement(address: number): ResultElement {
    const dataView = new DataView(this.instance.exports.memory.buffer);

    const typePtr = dataView.getUint32(address, true);
    const type = this.readString(typePtr);

    let text: string | undefined;
    const textPtr = dataView.getUint32(address + 4, true);
    if (textPtr !== 0) {
      text = this.readString(textPtr);
    } else {
      text = undefined;
    }

    let children: ResultElement[] | undefined;
    const childrenPtr = dataView.getUint32(address + 8, true);
    if (childrenPtr !== 0) {
      const childrenArray = this.readDynamicPointerArray(childrenPtr);
      children = childrenArray.map((childAddr) => this.readElement(childAddr));
    }

    let onClick: (() => void) | undefined;
    const onClickPtr = dataView.getUint32(address + 12, true);
    if (onClickPtr !== 0) {
      onClick = () => {
        this.instance.exports.invoke_on_click(address);
      };
    }

    return { type, text, children, onClick };
  }

  readDynamicPointerArray(address: number) {
    const dataView = new DataView(this.instance.exports.memory.buffer);
    const size = dataView.getUint32(address, true);
    const itemsPtr = dataView.getUint32(address + 8, true);

    const items = new Uint32Array(this.instance.exports.memory.buffer, itemsPtr, size);
    return Array.from(items);
  }

  readString(address: number) {
    if (address === 0) {
      throw new Error("Null pointer dereference");
    }

    const dataView = new DataView(this.instance.exports.memory.buffer);
    const stringBytes: number[] = [];
    let byte: number;
    while ((byte = dataView.getUint8(address + stringBytes.length))) {
      stringBytes.push(byte);
    }
    return decoder.decode(new Uint8Array(stringBytes));
  }
}
