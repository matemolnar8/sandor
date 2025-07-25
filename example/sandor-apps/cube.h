#ifndef CUBE_H
#define CUBE_H

#define CUBE_PI 3.14159265359f
#define CUBE_BACKGROUND_COLOR 0xFF181818
#define CUBE_GRID_COUNT 10
#define CUBE_GRID_PAD 0.5f/CUBE_GRID_COUNT
#define CUBE_GRID_SIZE ((CUBE_GRID_COUNT - 1)*CUBE_GRID_PAD)
#define CUBE_CIRCLE_RADIUS 4
#define CUBE_Z_START 0.4f

// Math functions provided by the platform
float sqrtf(float x);
float atan2f(float y, float x);
float sinf(float x);
float cosf(float x);

static float cube_angle = 0;

Olivec_Canvas render_cube(float dt, uint32_t* pixels, int width, int height)
{
    cube_angle += 0.25f*CUBE_PI*dt;

    Olivec_Canvas oc = olivec_canvas(pixels, width, height, width);

    olivec_fill(oc, CUBE_BACKGROUND_COLOR);
    for (int ix = 0; ix < CUBE_GRID_COUNT; ++ix) {
        for (int iy = 0; iy < CUBE_GRID_COUNT; ++iy) {
            for (int iz = 0; iz < CUBE_GRID_COUNT; ++iz) {
                float x = ix*CUBE_GRID_PAD - CUBE_GRID_SIZE/2;
                float y = iy*CUBE_GRID_PAD - CUBE_GRID_SIZE/2;
                float z = CUBE_Z_START + iz*CUBE_GRID_PAD;

                float cx = 0.0f;
                float cz = CUBE_Z_START + CUBE_GRID_SIZE/2;

                float dx = x - cx;
                float dz = z - cz;

                float a = atan2f(dz, dx);
                float m = sqrtf(dx*dx + dz*dz);

                dx = cosf(a + cube_angle)*m;
                dz = sinf(a + cube_angle)*m;

                x = dx + cx;
                z = dz + cz;

                x /= z;
                y /= z;

                uint32_t r = ix*255/CUBE_GRID_COUNT;
                uint32_t g = iy*255/CUBE_GRID_COUNT;
                uint32_t b = iz*255/CUBE_GRID_COUNT;
                uint32_t color = 0xFF000000 | (r<<(0*8)) | (g<<(1*8)) | (b<<(2*8));
                olivec_circle(oc, (x + 1)/2*width, (y + 1)/2*height, CUBE_CIRCLE_RADIUS, color);
            }
        }
    }

    return oc;
}

#endif // CUBE_H
