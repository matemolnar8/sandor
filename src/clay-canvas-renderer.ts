import {
  textConfigDefinition,
  renderCommandDefinition,
  rectangleConfigDefinition,
  borderConfigDefinition,
} from "./definitions.js";
import { readStructValueAtAddress, StructValue } from "./structs.js";
import { assertAndGet } from "./util/assert-value.js";

const textDecoder = new TextDecoder("utf-8");
const fontsById = ["Arial"];
const scale = 1;
const GLOBAL_FONT_SCALING_FACTOR = 1;
// const CLAY_RENDER_COMMAND_TYPE_NONE = 0;
const CLAY_RENDER_COMMAND_TYPE_RECTANGLE = 1;
const CLAY_RENDER_COMMAND_TYPE_BORDER = 2;
const CLAY_RENDER_COMMAND_TYPE_TEXT = 3;
// const CLAY_RENDER_COMMAND_TYPE_IMAGE = 4;
// const CLAY_RENDER_COMMAND_TYPE_SCISSOR_START = 5;
// const CLAY_RENDER_COMMAND_TYPE_SCISSOR_END = 6;
// const CLAY_RENDER_COMMAND_TYPE_CUSTOM = 7;

type ClayWasmInstance = {
  exports: {
    memory: WebAssembly.Memory;
    init: () => number;
    render_component: (width: number, height: number, deltaTime: number) => void;
    get_render_commands_length: () => number;
    get_render_command: (index: number) => number;
  };
};

export class ClayCanvasRenderer {
  #instance: (WebAssembly.Instance & ClayWasmInstance) | undefined;
  #memoryDataView: DataView | undefined;
  wasmPath: string;
  canvas: HTMLCanvasElement;
  canvasCtx: CanvasRenderingContext2D;
  previousFrameTime = 0;

  constructor(wasmPath: string, canvas: HTMLCanvasElement, ctx: CanvasRenderingContext2D) {
    this.wasmPath = wasmPath;
    this.canvas = canvas;
    this.canvasCtx = ctx;
  }

  get instance() {
    return assertAndGet(this.#instance, "Instance not found");
  }

  get memoryDataView() {
    return assertAndGet(this.#memoryDataView, "Memory data view not found");
  }

  async init() {
    this.canvasCtx.scale(1 / scale, 1 / scale);

    this.#instance = (
      await WebAssembly.instantiateStreaming(fetch("./hello.wasm"), {
        debug: {
          print: (a: number, b: number, c: number) => {
            console.log(a, b, c);
          },
        },
        clay: {
          measureTextFunction: (
            addressOfDimensions: number,
            addressOfTextToMeasure: number,
            addressOfConfig: number
          ) => {
            let stringLength = this.memoryDataView.getUint32(addressOfTextToMeasure, true);
            let pointerToString = this.memoryDataView.getUint32(addressOfTextToMeasure + 4, true);
            let textConfig = readStructValueAtAddress(addressOfConfig, textConfigDefinition, this.memoryDataView);
            let textDecoder = new TextDecoder("utf-8");
            let text = textDecoder.decode(
              this.memoryDataView.buffer.slice(pointerToString, pointerToString + stringLength)
            );
            let sourceDimensions = this.getTextDimensions(
              text,
              `${Math.round(textConfig.fontSize.value * GLOBAL_FONT_SCALING_FACTOR)}px ${
                fontsById[textConfig.fontId.value]
              }`
            );
            this.memoryDataView.setFloat32(addressOfDimensions, sourceDimensions.width, true);
            this.memoryDataView.setFloat32(addressOfDimensions + 4, sourceDimensions.height, true);
          },
        },
      })
    ).instance as ClayWasmInstance;

    this.#memoryDataView = new DataView(new Uint8Array(this.instance.exports.memory.buffer).buffer);

    const initResult = this.instance.exports.init();

    if (initResult !== 0) {
      console.error(`Failed to initialize ClayCanvasRenderer, code: ${initResult}`);
      return;
    }
  }

  getTextDimensions(text: string, font: string) {
    this.canvasCtx.font = font;
    let metrics = this.canvasCtx.measureText(text);
    return { width: metrics.width, height: metrics.fontBoundingBoxAscent + metrics.fontBoundingBoxDescent };
  }

  #handleTextCommand(renderCommand: StructValue<typeof renderCommandDefinition>) {
    const config = readStructValueAtAddress(renderCommand.config.value, textConfigDefinition, this.memoryDataView);
    const textContents = renderCommand.text;
    const boundingBox = renderCommand.boundingBox;
    const stringContents = new Uint8Array(
      this.memoryDataView.buffer.slice(textContents.chars.value, textContents.chars.value + textContents.length.value)
    );

    const fontSize = config.fontSize.value * GLOBAL_FONT_SCALING_FACTOR * scale;
    this.canvasCtx.font = `${fontSize}px ${fontsById[config.fontId.value]}`;
    const color = config.textColor;
    this.canvasCtx.textBaseline = "middle";
    this.canvasCtx.fillStyle = `rgba(${color.r.value}, ${color.g.value}, ${color.b.value}, ${color.a.value / 255})`;
    this.canvasCtx.fillText(
      textDecoder.decode(stringContents),
      boundingBox.x.value * scale,
      (boundingBox.y.value + boundingBox.height.value / 2 + 1) * scale
    );
  }

  #handleRectangleCommand(renderCommand: StructValue<typeof renderCommandDefinition>) {
    const config = readStructValueAtAddress(renderCommand.config.value, rectangleConfigDefinition, this.memoryDataView);
    const color = config.color;
    const boundingBox = renderCommand.boundingBox;

    this.canvasCtx.beginPath();
    this.canvasCtx.fillStyle = `rgba(${color.r.value}, ${color.g.value}, ${color.b.value}, ${color.a.value / 255})`;
    this.canvasCtx.roundRect(
      boundingBox.x.value * scale,
      boundingBox.y.value * scale,
      boundingBox.width.value * scale,
      boundingBox.height.value * scale,
      [
        config.cornerRadius.topLeft.value * scale,
        config.cornerRadius.topRight.value * scale,
        config.cornerRadius.bottomRight.value * scale,
        config.cornerRadius.bottomLeft.value * scale,
      ]
    );
    this.canvasCtx.fill();
    this.canvasCtx.closePath();
  }

  #handleBorderCommand(renderCommand: StructValue<typeof renderCommandDefinition>) {
    const config = readStructValueAtAddress(renderCommand.config.value, borderConfigDefinition, this.memoryDataView);
    const boundingBox = renderCommand.boundingBox;
    const cr = config.cornerRadius;

    const drawRoundedRect = (x: number, y: number, width: number, height: number, radius: number) => {
      this.canvasCtx.beginPath();
      this.canvasCtx.moveTo(x + radius, y);
      this.canvasCtx.lineTo(x + width - radius, y);
      this.canvasCtx.quadraticCurveTo(x + width, y, x + width, y + radius);
      this.canvasCtx.lineTo(x + width, y + height - radius);
      this.canvasCtx.quadraticCurveTo(x + width, y + height, x + width - radius, y + height);
      this.canvasCtx.lineTo(x + radius, y + height);
      this.canvasCtx.quadraticCurveTo(x, y + height, x, y + height - radius);
      this.canvasCtx.lineTo(x, y + radius);
      this.canvasCtx.quadraticCurveTo(x, y, x + radius, y);
      this.canvasCtx.closePath();
    };

    const drawBorder = (border: typeof config.left, x: number, y: number, width: number, height: number, radius: number) => {
      if (border.width.value === 0) return;

      this.canvasCtx.lineWidth = border.width.value * scale;
      this.canvasCtx.strokeStyle = `rgba(${border.color.r.value}, ${border.color.g.value}, ${border.color.b.value}, ${border.color.a.value / 255})`;
      drawRoundedRect(x + this.canvasCtx.lineWidth / 2, y + this.canvasCtx.lineWidth / 2, width - this.canvasCtx.lineWidth, height - this.canvasCtx.lineWidth, radius);
      this.canvasCtx.stroke();
    };

    const x = boundingBox.x.value * scale;
    const y = boundingBox.y.value * scale;
    const width = boundingBox.width.value * scale;
    const height = boundingBox.height.value * scale;
    const radius = Math.min(cr.topLeft.value, cr.topRight.value, cr.bottomRight.value, cr.bottomLeft.value) * scale;

    drawBorder(config.top, x, y, width, height, radius);
    drawBorder(config.bottom, x, y, width, height, radius);
    drawBorder(config.left, x, y, width, height, radius);
    drawBorder(config.right, x, y, width, height, radius);
  }

  render(deltaTime: number) {
    window.performance.mark("start_wasm_render");
    this.instance.exports.render_component(this.canvas.width, this.canvas.height, deltaTime);
    window.performance.mark("finish_wasm_render");

    window.performance.mark("start_js_render");
    const renderCommandCount = this.instance.exports.get_render_commands_length();

    for (let i = 0; i < renderCommandCount; i++) {
      const command = this.instance.exports.get_render_command(i);
      const renderCommand = readStructValueAtAddress(command, renderCommandDefinition, this.memoryDataView);

      switch (renderCommand.commandType.value) {
        case CLAY_RENDER_COMMAND_TYPE_TEXT:
          this.#handleTextCommand(renderCommand);
          break;
        case CLAY_RENDER_COMMAND_TYPE_RECTANGLE:
          this.#handleRectangleCommand(renderCommand);
          break;
        case CLAY_RENDER_COMMAND_TYPE_BORDER:
          this.#handleBorderCommand(renderCommand);
          break;
        default:
          console.error("Unimplemented render command type", renderCommand.commandType.value);
          break;
      }
    }

    window.performance.mark("finish_js_render");

    window.performance.measure("wasm_render", "start_wasm_render", "finish_wasm_render");
    window.performance.measure("js_render", "start_js_render", "finish_js_render");
  }

  renderLoop(currentTime = 0) {
    const elapsed = currentTime - this.previousFrameTime;
    this.previousFrameTime = currentTime;
    this.render(elapsed / 1000);
    requestAnimationFrame((time) => this.renderLoop(time));
  }
}
