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
#include "images/formula3.h"
#include "images/formula4.h"
#include "images/formula5.h"
#include "images/kuba.h"
#include "led-matrix-c.h"
#include "images/logo.h"
enum {
    FORMULA1,
    FORMULA2,
    FORMULA3,
    FORMULA4,
    FORMULA5,
    KUBA,
    LOGO,
};
int *getPicture(int picture) {
    switch (picture) {
        case FORMULA1:
            return (int *)&formula1[0][0];
        case FORMULA2:
            return (int *)&formula2[0][0];
        case FORMULA3:
            return (int *)&formula3[0][0];
        case FORMULA4:
            return (int *)&formula4[0][0];
        case FORMULA5:
            return (int *)&formula5[0][0];
            case KUBA:
            return (int *)&kuba[0][0];
        case LOGO:
            return (int *)&logo[0][0];
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

void animatePicture(struct LedCanvas *canvas, struct RGBLedMatrix *matrix, int picture) {
    int *payload = getPicture(picture);
    int frame[64][64 * 3];
    for (int anima = 0; anima < 64; anima++) {
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 64 * 3; j++) {
                frame[i][j] = 0;
            }
        }
        for (int y = 0; y < 64; y++) {
            for (int x = anima; x >= 0; x--) {
                int xDisplay = anima * 3 - x * 3;
                frame[y][xDisplay] = payload[y * 64 * 3 + x * 3];
                frame[y][xDisplay + 1] = payload[y * 64 * 3 + x * 3 + 1];
                frame[y][xDisplay + 2] = payload[y * 64 * 3 + x * 3 + 2];
            }
        }
        for (int y = 0; y < 64; y++) {
            for (int x = 0; x < 64; x++) {
                fprintf(stdout, "%d %d %d ;", frame[y][x * 3], frame[y][x * 3 + 1], frame[y][x * 3 + 2]);
                led_canvas_set_pixel(canvas, x, y, frame[y][x * 3], frame[y][x * 3 + 1], frame[y][x * 3 + 2]);
            }
            fprintf(stdout, "\n");
        }

        canvas = led_matrix_swap_on_vsync(matrix, canvas);
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

    for (int i = FORMULA1; i <= KUBA; i++) {
        showPicture(offscreen_canvas, matrix, i);
        sleep(5);
    }

    led_matrix_delete(matrix);

    return 0;
}
