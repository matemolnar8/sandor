#include "low_budget_memory.c"
#define CLAY_IMPLEMENTATION
#include "clay.h"

const Clay_Color COLOR_BLACK = (Clay_Color){0, 0, 0, 255};
const Clay_Color COLOR_WHITE = (Clay_Color){255, 255, 255, 255};
const Clay_Color COLOR_RED = (Clay_Color){255, 0, 0, 255};

Clay_TextElementConfig headerTextConfig = (Clay_TextElementConfig){.fontId = 0, .fontSize = 36, .textColor = COLOR_RED};

Clay_RenderCommandArray renderCommands;

Clay_Color ColorLerp(Clay_Color a, Clay_Color b, float amount)
{
    return (Clay_Color){
        .r = a.r + (b.r - a.r) * amount,
        .g = a.g + (b.g - a.g) * amount,
        .b = a.b + (b.b - a.b) * amount,
        .a = a.a + (b.a - a.a) * amount,
    };
}

__attribute__((import_module("debug"), import_name("print"))) void debugPrint(int int1, int int2, float float1);

CLAY_WASM_EXPORT("init")
int init(int width, int height)
{
    uint64_t totalMemorySize = Clay_MinMemorySize();
    void *memory = malloc(totalMemorySize);
    if (memory == NULL)
    {
        return 69;
    }

    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, memory);
    Clay_Initialize(arena, (Clay_Dimensions){width, height});

    return 0;
}

float animationLerpValue = -1.0f;

CLAY_WASM_EXPORT("render_component")
int render_component(int width, int height, float deltaTime)
{
    animationLerpValue += deltaTime * 0.5f;
    if (animationLerpValue > 1)
    {
        animationLerpValue -= 2;
    }

    float lerpValue = animationLerpValue < 0 ? (animationLerpValue + 1) : (1 - animationLerpValue);
    Clay_BeginLayout();

    CLAY(CLAY_ID("OuterContainer"), CLAY_LAYOUT({.layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = {CLAY_SIZING_FIXED(width), CLAY_SIZING_FIXED(height)}}),
         CLAY_RECTANGLE({.color = ColorLerp(COLOR_BLACK, COLOR_WHITE, lerpValue), .cornerRadius = CLAY_CORNER_RADIUS(10)}))
    {
        {
            CLAY(CLAY_ID("Header"), CLAY_LAYOUT({.layoutDirection = CLAY_LEFT_TO_RIGHT, .sizing = {CLAY_SIZING_GROW({}), CLAY_SIZING_FIT({})}}),
                 CLAY_RECTANGLE({.color = COLOR_WHITE, .cornerRadius = CLAY_CORNER_RADIUS(10)}),
                 CLAY_BORDER({.top = {2, COLOR_RED}, .bottom = {2, COLOR_RED}, .left = {2, COLOR_RED}, .right = {2, COLOR_RED}, .cornerRadius = CLAY_CORNER_RADIUS(30)}))
            {
                CLAY_TEXT(CLAY_STRING("Left"), &headerTextConfig);
                CLAY(CLAY_LAYOUT({.sizing = {CLAY_SIZING_GROW({}), CLAY_SIZING_FIT({})}}));
                CLAY_TEXT(CLAY_STRING("Right"), &headerTextConfig);
            }
            CLAY_TEXT(CLAY_STRING("Hello"), CLAY_TEXT_CONFIG({.fontId = 0, .fontSize = 24, .textColor = ColorLerp(COLOR_WHITE, COLOR_BLACK, lerpValue)}));
            CLAY_TEXT(CLAY_STRING("World"), CLAY_TEXT_CONFIG({.fontId = 0, .fontSize = 24, .textColor = ColorLerp(COLOR_WHITE, COLOR_BLACK, lerpValue)}));
        }
    }

    renderCommands = Clay_EndLayout();

    return 0;
}

CLAY_WASM_EXPORT("get_render_commands_length")
int get_render_commands_length()
{
    return renderCommands.length;
}

CLAY_WASM_EXPORT("get_render_command")
Clay_RenderCommand *get_render_command(int index)
{
    return Clay_RenderCommandArray_Get(&renderCommands, index);
}
