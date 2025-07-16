#ifndef DVD_H
#define DVD_H

#define DVD_BACKGROUND_COLOR 0xFF181818
#define DVD_SQUARE_SIZE 30
#define DVD_SPEED 100.0f

uint32_t dvd_pixels[WIDTH*HEIGHT];
Olivec_Canvas dvd_canvas = OLIVEC_CANVAS_NULL;

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

Olivec_Canvas render_dvd(float dt)
{
    // Update position
    dvd_x += dvd_dx * dt;
    dvd_y += dvd_dy * dt;
    
    // Bounce off walls and change color
    if (dvd_x <= 0 || dvd_x >= WIDTH - DVD_SQUARE_SIZE) {
        dvd_dx = -dvd_dx;
        dvd_x = (dvd_x <= 0) ? 0 : WIDTH - DVD_SQUARE_SIZE;
        dvd_change_color();
    }
    
    if (dvd_y <= 0 || dvd_y >= HEIGHT - DVD_SQUARE_SIZE) {
        dvd_dy = -dvd_dy;
        dvd_y = (dvd_y <= 0) ? 0 : HEIGHT - DVD_SQUARE_SIZE;
        dvd_change_color();
    }
    
    Olivec_Canvas oc = olivec_canvas(dvd_pixels, WIDTH, HEIGHT, WIDTH);
    
    // Clear background
    olivec_fill(oc, DVD_BACKGROUND_COLOR);
    
    // Draw simple colored square
    int square_x = (int)dvd_x;
    int square_y = (int)dvd_y;
    
    olivec_rect(oc, square_x, square_y, DVD_SQUARE_SIZE, DVD_SQUARE_SIZE, dvd_color);
    
    return oc;
}

#endif // DVD_H
