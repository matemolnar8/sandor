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
#define DVD_SPEED 100.0f

static float dvd_x = 50.0f;
static float dvd_y = 50.0f;
static float dvd_dx = DVD_SPEED;
static float dvd_dy = DVD_SPEED;
static uint32_t dvd_color = 0xFF00FF00; // Start with green
static bool dvd_tint_dirty = true;

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

// Working copy of the logo texture, recolored to the current bounce color.
static uint32_t dvd_logo_tinted_pixels[DVD_LOGO_WIDTH * DVD_LOGO_HEIGHT];

void dvd_change_color() {
    dvd_color_index = (dvd_color_index + 1) % (sizeof(dvd_colors) / sizeof(dvd_colors[0]));
    dvd_color = dvd_colors[dvd_color_index];
    dvd_tint_dirty = true;
}

static void dvd_update_tinted_logo(void)
{
    if (!dvd_tint_dirty) return;

    uint32_t tr = OLIVEC_RED(dvd_color);
    uint32_t tg = OLIVEC_GREEN(dvd_color);
    uint32_t tb = OLIVEC_BLUE(dvd_color);
    size_t n = DVD_LOGO_WIDTH * DVD_LOGO_HEIGHT;

    for (size_t i = 0; i < n; i++) {
        uint32_t src = dvd_logo_pixels[i];
        uint32_t a = OLIVEC_ALPHA(src);
        // Keep logo coverage (alpha), replace RGB with the bounce tint.
        dvd_logo_tinted_pixels[i] = (a << 24) | (tb << 16) | (tg << 8) | tr;
    }

    dvd_tint_dirty = false;
}

Olivec_Canvas render_dvd(float dt, uint32_t* pixels, int width, int height)
{
    int logo_w = DVD_LOGO_WIDTH;
    int logo_h = DVD_LOGO_HEIGHT;

    dvd_x += dvd_dx * dt;
    dvd_y += dvd_dy * dt;

    if (dvd_x <= 0 || dvd_x >= width - logo_w) {
        dvd_dx = -dvd_dx;
        dvd_x = (dvd_x <= 0) ? 0 : width - logo_w;
        dvd_change_color();
    }

    if (dvd_y <= 0 || dvd_y >= height - logo_h) {
        dvd_dy = -dvd_dy;
        dvd_y = (dvd_y <= 0) ? 0 : height - logo_h;
        dvd_change_color();
    }

    Olivec_Canvas oc = olivec_canvas(pixels, width, height, width);
    olivec_fill(oc, DVD_BACKGROUND_COLOR);

    dvd_update_tinted_logo();
    Olivec_Canvas logo = olivec_canvas(
        dvd_logo_tinted_pixels,
        DVD_LOGO_WIDTH,
        DVD_LOGO_HEIGHT,
        DVD_LOGO_WIDTH
    );
    olivec_sprite_blend(oc, (int)dvd_x, (int)dvd_y, logo_w, logo_h, logo);

    return oc;
}

#endif // DVD_H
