import { CPrimitiveTypeDefinition, CStructDefinition, CPrimitiveType } from "./definitions";
import { Prettify } from "./util/type-helper";

type PrimitiveValue<T extends CPrimitiveType> = {
  value: T extends "float" ? number :
         T extends "uint32_t" ? number :
         T extends "uint16_t" ? number :
         T extends "uint8_t" ? number :
         T extends "bool" ? boolean :
         never;
  __size: number;
};

type ExtractValue<T extends CPrimitiveTypeDefinition | CStructDefinition> = 
  T extends CPrimitiveTypeDefinition 
    ? PrimitiveValue<T["type"]>
    : T extends CStructDefinition
      ? StructValue<T>
      : never;

export type StructValue<T extends CStructDefinition> = Prettify<
  { __size: number } & {
    [K in T["members"][number]["name"]]: ExtractValue<Extract<T["members"][number], { name: K }>>
  }
>;

export function getStructTotalSize(definition: CStructDefinition | CPrimitiveTypeDefinition) {
  switch (definition.type) {
    case "union":
    case "struct": {
      let totalSize = 0;
      for (const member of definition.members) {
        let result = getStructTotalSize(member);
        if (definition.type === "struct") {
          totalSize += result;
        } else {
          totalSize = Math.max(totalSize, result);
        }
      }
      return totalSize;
    }
    case "float":
      return 4;
    case "uint32_t":
      return 4;
    case "uint16_t":
      return 2;
    case "uint8_t":
      return 1;
    case "bool":
      return 1;
  }
}

export function readPrimitiveValueAtAddress<T extends CPrimitiveTypeDefinition>(
  address: number,
  definition: T,
  memoryDataView: DataView
) {
  switch (definition.type) {
    case "float":
      return {
        value: memoryDataView.getFloat32(address, true),
        __size: 4,
      };
    case "uint32_t":
      return {
        value: memoryDataView.getUint32(address, true),
        __size: 4,
      };
    case "uint16_t":
      return {
        value: memoryDataView.getUint16(address, true),
        __size: 2,
      };
    case "uint8_t":
      return { value: memoryDataView.getUint8(address), __size: 1 };
    case "bool":
      return { value: !!memoryDataView.getUint8(address), __size: 1 };
  }
}

export function readStructValueAtAddress<T extends CStructDefinition>(
  address: number,
  definition: T,
  memoryDataView: DataView
): StructValue<T> {
  let struct: { __size: number } & Partial<Record<string, any>> = { __size: 0 };
  for (const member of definition.members) {
    let result;

    if (member.type === "struct" || member.type === "union") {
      result = readStructValueAtAddress(address, member, memoryDataView);
    } else {
      result = readPrimitiveValueAtAddress(address, member as CPrimitiveTypeDefinition, memoryDataView);
    }

    if (definition.type === "struct") {
      struct.__size += result.__size;
      address += result.__size;
    } else {
      struct.__size = Math.max(struct.__size, result.__size);
    }

    struct[member.name] = result;
  }
  return struct as StructValue<T>;
}
