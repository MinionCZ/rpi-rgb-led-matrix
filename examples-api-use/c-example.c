/* -*- mode: c; c-basic-offset: 2; indent-tabs-mode: nil; -*-
 *
 * Using the C-API of this library.
 *
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "images/formula1.h"
#include "images/formula2.h"
#include "images/kuba1.h"
#include "images/kuba2.h"
#include "images/logo.h"
#include "led-matrix-c.h"
enum {
    FORMULA1,
    FORMULA2,
};
int *getPicture(int picture) {
    switch (picture) {
        case FORMULA1:
            return (int *)&formula1[0][0];
        case FORMULA2:
            return (int *)&formula2[0][0];
    }
    return NULL;
}

void showPicture(struct LedCanvas *canvas, struct RGBLedMatrix *matrix, int picture) {
    int *payload = getPicture(picture);
    for (int y = 0; y < 64; ++y) {
        for (int x = 0; x < 64; ++x) {
            led_canvas_set_pixel(canvas, x, y, payload[y * 64 * 3 + x * 3], payload[y * 64 * 3 + x * 3 + 1], payload[y * 64 * 3 + x * 3 + 2]);
        }
    }
    canvas = led_matrix_swap_on_vsync(matrix, canvas);
}

void animatePicture(int picture, struct LedCanvas *canvas, struct RGBLedMatrix *matrix) {
    int *payload = getPicture(picture);
    for (int maxX = 63 * 3; maxX >= 0; maxX -= 3) {
        for (int y = 0; y < 64; y++) {
            for (int x = 63 * 3; x >= maxX; x -= 3) {
                led_canvas_set_pixel(canvas, x, y, payload[y * 64 * 3 + x * 3], payload[y * 64 * 3 + x * 3 + 1], payload[y * 64 * 3 + x * 3 + 2]);
            }
            for (int x = maxX - 3; x >= 0; x -= 3) {
                led_canvas_set_pixel(canvas, x, y, 0, 0, 0);
            }
        }
        canvas = led_matrix_swap_on_vsync(matrix, canvas);
        usleep(50);
    }
}

int main(int argc, char **argv) {
    struct RGBLedMatrixOptions options;
    struct RGBLedMatrix *matrix;
    struct LedCanvas *offscreen_canvas;
    int width, height;

    memset(&options, 0, sizeof(options));
    options.rows = 64;
    options.cols = 64;
    options.chain_length = 1;

    matrix = led_matrix_create_from_options(&options, &argc, &argv);
    if (matrix == NULL)
        return 1;

    offscreen_canvas = led_matrix_create_offscreen_canvas(matrix);

    led_canvas_get_size(offscreen_canvas, &width, &height);

    fprintf(stderr, "Size: %dx%d. Hardware gpio mapping: %s\n", width, height, options.hardware_mapping);

    for (int i = FORMULA1; i <= FORMULA2; i++) {
        showPicture(offscreen_canvas, matrix, i);
        sleep(5);
    }

    led_matrix_delete(matrix);

    return 0;
}
