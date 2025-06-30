import { assertAndGet } from "./util/assert-value.js";
import morphdom from "morphdom";

type WasmInstance = {
  exports: {
    memory: WebAssembly.Memory;
    render_component: () => number;
    invoke_on_click: (elementIndex: number) => void;
    invoke_on_change: (elementIndex: number, valuePtr: number) => void;
    get_input_buffer: () => number;
    get_element_layout: () => number;
  };
};

const decoder = new TextDecoder();

// Element type constants matching C enum
const ElementType = {
  GENERIC: 0,
  BUTTON: 1,
  INPUT: 2,
} as const;

type ElementTypeKeys = "generic" | "button" | "input";
type ElementSpecificOffsets = {
  generic: { tag: number };
  button: { onClick: number; onClickArgs: number };
  input: { placeholder: number; onChange: number };
};
type ElementOffsets = {
  type: number;
  index: number;
  text: number;
  children: number;
  attributes: number;
  union: number;
} & { [K in ElementTypeKeys]: ElementSpecificOffsets[K] };

type ElementSpecificProps = {
  generic: {
    tag: string;
  };
  button: {
    onClick: () => void;
  };
  input: {
    placeholder: string;
    onChange: (value: string) => void;
  };
};

export type ResultElement = {
  elementType: ElementTypeKeys;
  id: number;
  text?: string;
  children?: ResultElement[];
  attributes?: Record<string, string>;
} & {
  [K in ElementTypeKeys]: {
    elementType: K;
  } & ElementSpecificProps[K];
}[ElementTypeKeys];

declare global {
  interface Window {
    sandor?: {
      debug?: boolean;
    };
  }
}

export class WasmComponent {
  #instance: (WebAssembly.Instance & WasmInstance) | undefined;
  #memoryDataView: DataView | undefined;
  #elementOffsets: ElementOffsets | undefined;
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

  get elementOffsets() {
    return assertAndGet(this.#elementOffsets, "Element offsets not found");
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

    // Read layout offsets from WASM memory
    const layoutPtr = this.instance.exports.get_element_layout();
    const layoutView = new DataView(this.instance.exports.memory.buffer);
    const sizeOfSizeT = 4;

    // Layout array indices (must match the C array order)
    const LAYOUT = {
      TYPE: 0,
      INDEX: 1,
      TEXT: 2,
      CHILDREN: 3,
      ATTRIBUTES: 4,
      UNION: 5,
      GENERIC_TAG: 6,
      BUTTON_ON_CLICK: 7,
      BUTTON_ON_CLICK_ARGS: 8,
      INPUT_PLACEHOLDER: 9,
      INPUT_ON_CHANGE: 10,
    };

    this.#elementOffsets = {
      type: layoutView.getUint32(layoutPtr + LAYOUT.TYPE * sizeOfSizeT, true),
      index: layoutView.getUint32(layoutPtr + LAYOUT.INDEX * sizeOfSizeT, true),
      text: layoutView.getUint32(layoutPtr + LAYOUT.TEXT * sizeOfSizeT, true),
      children: layoutView.getUint32(layoutPtr + LAYOUT.CHILDREN * sizeOfSizeT, true),
      attributes: layoutView.getUint32(layoutPtr + LAYOUT.ATTRIBUTES * sizeOfSizeT, true),
      union: layoutView.getUint32(layoutPtr + LAYOUT.UNION * sizeOfSizeT, true),
      generic: {
        tag: layoutView.getUint32(layoutPtr + LAYOUT.GENERIC_TAG * sizeOfSizeT, true),
      },
      button: {
        onClick: layoutView.getUint32(layoutPtr + LAYOUT.BUTTON_ON_CLICK * sizeOfSizeT, true),
        onClickArgs: layoutView.getUint32(layoutPtr + LAYOUT.BUTTON_ON_CLICK_ARGS * sizeOfSizeT, true),
      },
      input: {
        placeholder: layoutView.getUint32(layoutPtr + LAYOUT.INPUT_PLACEHOLDER * sizeOfSizeT, true),
        onChange: layoutView.getUint32(layoutPtr + LAYOUT.INPUT_ON_CHANGE * sizeOfSizeT, true),
      },
    };
  }

  render() {
    const resultAddr = this.instance.exports.render_component();
    const root = this.readElement(resultAddr);

    if (!this.parent) {
      return;
    }

    const newRootElement = document.createElement("div");
    newRootElement.setAttribute("data-instance-id", this.instanceId);
    newRootElement.setAttribute("class", "h-full flex flex-col");

    const renderElement = (renderResult: ResultElement, parentElement: HTMLElement) => {
      let element: HTMLElement;

      // Create element based on type
      switch (renderResult.elementType) {
        case "generic":
          element = document.createElement(renderResult.tag);
          break;

        case "button":
          element = document.createElement("button");
          element.id = `sandor-btn-${renderResult.id}`;
          element.addEventListener("click", renderResult.onClick);
          break;

        case "input":
          element = document.createElement("input");
          element.id = `sandor-input-${renderResult.id}`;
          element.setAttribute("type", "text");
          element.setAttribute("placeholder", renderResult.placeholder);
          if (renderResult.text) {
            element.setAttribute("value", renderResult.text);
          }
          element.addEventListener("change", (event) => {
            const target = event.target as HTMLInputElement;
            renderResult.onChange(target.value);
          });
          break;
      }

      // Apply common properties
      if (renderResult.text) {
        element.textContent = renderResult.text;
      }

      if (renderResult.children) {
        renderResult.children.forEach((childResult) => renderElement(childResult, element));
      }

      // Apply attributes
      if (renderResult.attributes) {
        for (const [key, value] of Object.entries(renderResult.attributes)) {
          element.setAttribute(key, value);
        }
      }

      parentElement.appendChild(element);
    };

    renderElement(root, newRootElement);

    if (window.sandor?.debug) {
      const debugRerenderButton = document.createElement("button");
      debugRerenderButton.classList.add("btn", "rounded-full");
      debugRerenderButton.textContent = "Debug rerender";
      debugRerenderButton.addEventListener("click", () => {
        this.render();
      });
      newRootElement.appendChild(debugRerenderButton);
    }

    const existingRootElement = this.parent.querySelector(`[data-instance-id="${this.instanceId}"]`);
    if (!existingRootElement) {
      this.parent.appendChild(newRootElement);
    } else {
      morphdom(existingRootElement, newRootElement);
    }
  }

  readElement(address: number): ResultElement {
    const dataView = new DataView(this.instance.exports.memory.buffer);
    const offsets = this.elementOffsets;

    // Read common fields
    const elementType = dataView.getUint32(address + offsets.type, true);
    const id = dataView.getUint32(address + offsets.index, true);

    // Read common properties
    let text: string | undefined;
    const textPtr = dataView.getUint32(address + offsets.text, true);
    if (textPtr !== 0) {
      text = this.readString(textPtr);
    }

    let children: ResultElement[] | undefined;
    const childrenPtr = dataView.getUint32(address + offsets.children, true);
    if (childrenPtr !== 0) {
      const childrenArray = this.readDynamicPointerArray(childrenPtr);
      children = childrenArray.map((childAddr) => this.readElement(childAddr));
    }

    let attributes: Record<string, string> | undefined;
    const attributesPtr = dataView.getUint32(address + offsets.attributes, true);
    if (attributesPtr !== 0) {
      const attributesArray = this.readDynamicPointerArray(attributesPtr);
      attributes = {};
      for (let i = 0; i < attributesArray.length; i++) {
        const attributePtr = attributesArray[i];
        const keyPtr = this.memoryDataView.getUint32(attributePtr, true);
        const valuePtr = this.memoryDataView.getUint32(attributePtr + 4, true);
        const key = this.readString(keyPtr);
        const value = this.readString(valuePtr);
        attributes[key] = value;
      }
    }

    const unionAddress = address + offsets.union;

    // Handle different element types
    switch (elementType) {
      case ElementType.GENERIC: {
        const tagPtr = dataView.getUint32(unionAddress + offsets.generic.tag, true);
        const tag = this.readString(tagPtr);

        return {
          elementType: "generic",
          id,
          tag,
          text,
          children,
          attributes,
        };
      }

      case ElementType.BUTTON: {
        const onClickPtr = dataView.getUint32(unionAddress + offsets.button.onClick, true);
        const onClick =
          onClickPtr !== 0
            ? () => {
                this.instance.exports.invoke_on_click(id);
              }
            : () => {};

        return {
          elementType: "button",
          id,
          text,
          children,
          attributes,
          onClick,
        };
      }

      case ElementType.INPUT: {
        const placeholderPtr = dataView.getUint32(unionAddress + offsets.input.placeholder, true);
        const placeholder = this.readString(placeholderPtr);

        const onChangePtr = dataView.getUint32(unionAddress + offsets.input.onChange, true);
        const onChange =
          onChangePtr !== 0
            ? (newValue: string) => {
                const bufferPtr = this.instance.exports.get_input_buffer();
                const valuePtr = this.writeString(newValue, bufferPtr);
                this.instance.exports.invoke_on_change(id, valuePtr);
              }
            : () => {};

        return {
          elementType: "input",
          id,
          placeholder,
          text, // For input, text is the current value
          children,
          attributes,
          onChange,
        };
      }

      default:
        throw new Error(`Unknown element type: ${elementType}`);
    }
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

  writeString(value: string, bufferPtr: number) {
    const bytes = new TextEncoder().encode(value);
    const dataView = new DataView(this.instance.exports.memory.buffer);

    for (let i = 0; i < bytes.length; i++) {
      dataView.setUint8(bufferPtr + i, bytes[i]);
    }

    dataView.setUint8(bufferPtr + bytes.length, 0);

    return bufferPtr;
  }
}
