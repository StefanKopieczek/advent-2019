#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int *values;
    int count;
    int capacity;
} Pixels;

Pixels *pixels_load(FILE *);
Pixels *pixels_init();
void pixels_free(Pixels *);
void pixels_append(Pixels *, int value);
int pixels_get(Pixels *, int index);

typedef struct {
    Pixels *pixels;
    int width;
    int height;
    int layers;
} Image;

Image *image_init(Pixels *, int width, int height);
void image_free(Image *); // Must *not* free the pixels!
int image_get_pixel(Image *, int layer, int row, int col);

bool check_format(Pixels *, int width, int height);
int count_layers(Pixels *, int width, int height);

int count_digit_in_layer(Image *, int layer, int digit);
int get_layer_start(Image *, int layer);
int get_layer_end(Image *, int layer);

int get_checksum(Image *);

void render_image(Image *);
int get_effective_pixel(Image *, int width, int height);
void render_pixel(int pixel);
