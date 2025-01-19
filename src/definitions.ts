export type CPrimitiveType = "float" | "uint32_t" | "uint16_t" | "uint8_t" | "bool";
export type CStructType = "struct" | "union";

export type CPrimitiveTypeDefinition = {
  type: CPrimitiveType;
};

export type CStructDefinition = {
  type: CStructType;
  members: readonly MemberDefinition[];
};

export type MemberDefinition = {
  name: string;
} & (CPrimitiveTypeDefinition | CStructDefinition);

export const colorDefinition = {
  type: "struct",
  members: [
    { name: "r", type: "float" },
    { name: "g", type: "float" },
    { name: "b", type: "float" },
    { name: "a", type: "float" },
  ],
} as const;

export const stringDefinition = {
  type: "struct",
  members: [
    { name: "length", type: "uint32_t" },
    { name: "chars", type: "uint32_t" },
  ],
} as const;

export const borderDefinition = {
  type: "struct",
  members: [
    { name: "width", type: "uint32_t" },
    { name: "color", ...colorDefinition },
  ],
} as const;

export const cornerRadiusDefinition = {
  type: "struct",
  members: [
    { name: "topLeft", type: "float" },
    { name: "topRight", type: "float" },
    { name: "bottomLeft", type: "float" },
    { name: "bottomRight", type: "float" },
  ],
} as const;

export const rectangleConfigDefinition = {
  name: "rectangle",
  type: "struct",
  members: [
    { name: "color", ...colorDefinition },
    { name: "cornerRadius", ...cornerRadiusDefinition },
    { name: "link", ...stringDefinition },
    { name: "cursorPointer", type: "uint8_t" },
  ],
} as const;

export const borderConfigDefinition = {
  type: "struct",
  members: [
    { name: "left", ...borderDefinition },
    { name: "right", ...borderDefinition },
    { name: "top", ...borderDefinition },
    { name: "bottom", ...borderDefinition },
    { name: "betweenChildren", ...borderDefinition },
    { name: "cornerRadius", ...cornerRadiusDefinition },
  ],
} as const;

export const textConfigDefinition = {
  type: "struct",
  members: [
    { name: "textColor", ...colorDefinition },
    { name: "fontId", type: "uint16_t" },
    { name: "fontSize", type: "uint16_t" },
    { name: "letterSpacing", type: "uint16_t" },
    { name: "lineSpacing", type: "uint16_t" },
    { name: "wrapMode", type: "uint32_t" },
    { name: "disablePointerEvents", type: "uint8_t" },
  ],
} as const;

export const scrollConfigDefinition = {
  type: "struct",
  members: [
    { name: "horizontal", type: "bool" },
    { name: "vertical", type: "bool" },
  ],
} as const;

export const imageConfigDefinition = {
  type: "struct",
  members: [
    { name: "imageData", type: "uint32_t" },
    {
      name: "sourceDimensions",
      type: "struct",
      members: [
        { name: "width", type: "float" },
        { name: "height", type: "float" },
      ],
    },
    { name: "sourceURL", ...stringDefinition },
  ],
} as const;

export const customConfigDefinition = {
  type: "struct",
  members: [{ name: "customData", type: "uint32_t" }],
} as const;

export const renderCommandDefinition = {
  type: "struct",
  members: [
    {
      name: "boundingBox",
      type: "struct",
      members: [
        { name: "x", type: "float" },
        { name: "y", type: "float" },
        { name: "width", type: "float" },
        { name: "height", type: "float" },
      ],
    },
    { name: "config", type: "uint32_t" },
    { name: "text", ...stringDefinition },
    { name: "id", type: "uint32_t" },
    { name: "commandType", type: "uint32_t" },
  ],
} as const;
