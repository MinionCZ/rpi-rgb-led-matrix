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
    KUBA1,
    KUBA2,
    FORMULA1,
    FORMULA2,
    LOGO
};
int *getPicture(int picture) {
    switch (picture) {
        case KUBA1:
            return (int *) &kuba1[0][0];
        case KUBA2:
            return (int *)  &kuba2[0][0];
        case FORMULA1:
          return (int *)  &formula1[0][0];
        case FORMULA2:
          return (int *)  &formula2[0][0];
        case LOGO:
          return (int *)  &logo[0][0];
    }
    return NULL;
}

void showPicture(struct LedCanvas *canvas, struct RGBLedMatrix *matrix, int picture) {
  int * payload = getPicture(picture);
    for (int y = 0; y < 64; ++y) {
        for (int x = 0; x < 64; ++x) {
            led_canvas_set_pixel(canvas, x, y, payload[y * 64 * 3 + x * 3] , payload [y * 64 * 3 + x * 3 + 1], payload[y * 64 * 3 + x * 3 + 2]);
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

    for(int i = 0; i <= LOGO; i++){
      showPicture(offscreen_canvas, matrix, i);
    }

    led_matrix_delete(matrix);

    return 0;
}
