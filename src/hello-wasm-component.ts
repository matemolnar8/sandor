import { assertAndGet } from "./util/assert-value.js";

type WasmInstance = {
  exports: {
    memory: WebAssembly.Memory;
    render_component: () => number;
  };
};

const decoder = new TextDecoder();

export class HelloWasmComponent {
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
    const result = this.readString(resultAddr);

    return result;
  }

  readString(address: number) {
    const length = this.memoryDataView.getUint32(address, true);
    const data = new Uint8Array(this.instance.exports.memory.buffer, address + 4, length);

    return new TextDecoder().decode(data);
  }
}
