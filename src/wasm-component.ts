import { assertAndGet } from "./util/assert-value.js";

type WasmInstance = {
  exports: {
    memory: WebAssembly.Memory;
    render_component: () => number;
  };
};

const decoder = new TextDecoder();

export type ResultElement = {
  type: string;
  text?: string;
  children?: ResultElement[];
};

export class WasmComponent {
  #instance: (WebAssembly.Instance & WasmInstance) | undefined;
  #memoryDataView: DataView | undefined;
  wasmPath: string;

  constructor(wasmPath: string) {
    this.wasmPath = wasmPath;
  }

  get instance() {
    return assertAndGet(this.#instance, "Instance not found");
  }

  get memoryDataView() {
    return assertAndGet(this.#memoryDataView, "Memory data view not found");
  }

  async init() {
    this.#instance = (
      await WebAssembly.instantiateStreaming(fetch("./hello.wasm"), {
        env: {
          platform_write: (buf: number, len: number) => {
            const text = decoder.decode(new Uint8Array(this.instance.exports.memory.buffer, buf, len));
            console.log(text);
          },
        },
      })
    ).instance as WasmInstance;

    this.#memoryDataView = new DataView(new Uint8Array(this.instance.exports.memory.buffer).buffer);
  }

  render() {
    const resultAddr = this.instance.exports.render_component();
    const result = this.readElement(resultAddr);

    return result;
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

    return { type, text, children };
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
