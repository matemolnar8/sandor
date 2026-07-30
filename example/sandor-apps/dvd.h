#ifndef DVD_H
#define DVD_H

#include <stdint.h>

#ifndef OLIVE_C_
#ifndef OLIVEC_IMPLEMENTATION
#define OLIVEC_IMPLEMENTATION
#endif
#include "olive.c"
#endif

#include "dvd_logo.h"

#define DVD_BACKGROUND_COLOR 0xFF181818
#define DVD_LOGO_DRAW_WIDTH DVD_LOGO_WIDTH
#define DVD_LOGO_DRAW_HEIGHT DVD_LOGO_HEIGHT
#define DVD_SPEED 100.0f

static float dvd_x = 50.0f;
static float dvd_y = 50.0f;
static float dvd_dx = DVD_SPEED;
static float dvd_dy = DVD_SPEED;
static uint32_t dvd_color = 0xFF00FF00; // Start with green

uint32_t dvd_colors[] = {
    0xFF00FF00, // Green
    0xFFFF0000, // Red
    0xFF0000FF, // Blue
    0xFFFFFF00, // Yellow
    0xFFFF00FF, // Magenta
    0xFF00FFFF, // Cyan
    0xFFFFFFFF, // White
};
static int dvd_color_index = 0;

void dvd_change_color() {
    dvd_color_index = (dvd_color_index + 1) % (sizeof(dvd_colors) / sizeof(dvd_colors[0]));
    dvd_color = dvd_colors[dvd_color_index];
}

static void dvd_blit_logo_tinted(Olivec_Canvas oc, int x, int y, uint32_t tint)
{
    Olivec_Canvas sprite = olivec_canvas(dvd_logo_pixels, DVD_LOGO_WIDTH, DVD_LOGO_HEIGHT, DVD_LOGO_WIDTH);
    uint32_t tr = OLIVEC_RED(tint);
    uint32_t tg = OLIVEC_GREEN(tint);
    uint32_t tb = OLIVEC_BLUE(tint);

    for (int sy = 0; sy < DVD_LOGO_HEIGHT; sy++) {
        int dy = y + sy;
        if (dy < 0 || dy >= (int)oc.height) continue;
        for (int sx = 0; sx < DVD_LOGO_WIDTH; sx++) {
            int dx = x + sx;
            if (dx < 0 || dx >= (int)oc.width) continue;

            uint32_t src = OLIVEC_PIXEL(sprite, sx, sy);
            uint32_t sa = OLIVEC_ALPHA(src);
            if (sa == 0) continue;

            uint32_t tinted = (sa << 24) | (tb << 16) | (tg << 8) | tr;
            olivec_blend_color(&OLIVEC_PIXEL(oc, dx, dy), tinted);
        }
    }
}

Olivec_Canvas render_dvd(float dt, uint32_t* pixels, int width, int height)
{
    dvd_x += dvd_dx * dt;
    dvd_y += dvd_dy * dt;

    if (dvd_x <= 0 || dvd_x >= width - DVD_LOGO_DRAW_WIDTH) {
        dvd_dx = -dvd_dx;
        dvd_x = (dvd_x <= 0) ? 0 : width - DVD_LOGO_DRAW_WIDTH;
        dvd_change_color();
    }

    if (dvd_y <= 0 || dvd_y >= height - DVD_LOGO_DRAW_HEIGHT) {
        dvd_dy = -dvd_dy;
        dvd_y = (dvd_y <= 0) ? 0 : height - DVD_LOGO_DRAW_HEIGHT;
        dvd_change_color();
    }

    Olivec_Canvas oc = olivec_canvas(pixels, width, height, width);

    olivec_fill(oc, DVD_BACKGROUND_COLOR);
    dvd_blit_logo_tinted(oc, (int)dvd_x, (int)dvd_y, dvd_color);

    return oc;
}

#endif // DVD_H
