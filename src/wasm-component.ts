import { assertAndGet } from "./util/assert-value.js";
import morphdom from "morphdom";

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
  attributes?: Record<string, string>;
  id: number;
};

export class WasmComponent {
  #instance: (WebAssembly.Instance & WasmInstance) | undefined;
  #memoryDataView: DataView | undefined;
  wasmPath: string;
  parent: HTMLElement | undefined;
  instanceId = crypto.randomUUID();

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

    const newRootElement = document.createElement("div");
    newRootElement.setAttribute("data-instance-id", this.instanceId);

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

      if (renderResult.attributes) {
        for (const [key, value] of Object.entries(renderResult.attributes)) {
          element.setAttribute(key, value);
        }
      }

      parentElement.appendChild(element);
    };

    renderElement(root, newRootElement);

    const existingRootElement = this.parent.querySelector(`[data-instance-id="${this.instanceId}"]`);
    if (!existingRootElement) {
      this.parent.appendChild(newRootElement);
    } else {
      morphdom(existingRootElement, newRootElement);
    }
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
        this.instance.exports.invoke_on_click(id);
      };
    }

    let attributes: Record<string, string> | undefined;
    const attributesPtr = dataView.getUint32(address + 20, true);
    if (attributesPtr !== 0) {
      const attributesArray = this.readDynamicPointerArray(attributesPtr);
      attributes = {};
      for (let i = 0; i < attributesArray.length; i++) {
        const attributesPtr = attributesArray[i];
        const keyPtr = this.memoryDataView.getUint32(attributesPtr, true);
        const valuePtr = this.memoryDataView.getUint32(attributesPtr + 4, true);
        const key = this.readString(keyPtr);
        const value = this.readString(valuePtr);
        attributes[key] = value;
      }
    }

    const id = dataView.getUint32(address + 24, true);

    return { type, text, children, onClick, attributes, id };
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
