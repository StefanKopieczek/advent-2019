#include "8.h"
#define INPUT "./inputs/8.txt"
#define INITIAL_PIXELS_CAPACITY 50

int main(int argc, char **argv) {
    FILE *f = fopen(INPUT, "r");
    Pixels *pixels = pixels_load(f);
    fclose(f);

    Image *img = image_init(pixels, 25, 6);
    printf("The image checksum is: %d\n\n", get_checksum(img));
    render_image(img);

    image_free(img);
    pixels_free(pixels);
    return 0;
}

int get_checksum(Image *img) {
    int bestLayer = -1;
    int bestLayerZeros = img->width * img->height + 1;
    for (int layer = 0; layer < img->layers; layer++) {
        int layerZeros = count_digit_in_layer(img, layer, 0);
        if (layerZeros < bestLayerZeros) {
            bestLayer = layer;
            bestLayerZeros = layerZeros;
        }
    }

    int numOnes = count_digit_in_layer(img, bestLayer, 1);
    int numTwos = count_digit_in_layer(img, bestLayer, 2);
    return numOnes * numTwos;
};

void render_image(Image *img) {
    for (int row = 0; row < img->height; row++) {
        for (int col = 0; col < img->width; col++) {
            render_pixel(get_effective_pixel(img, row, col));
        }
        printf("\n");
    }
}

void render_pixel(int pixel) {
    switch (pixel) {
        case 0:
            printf("  ");
            break;
        case 1:
            printf("XX");
            break;
        default:
            printf("!!");
    }
}

int get_effective_pixel(Image *img, int row, int col) {
    int result = 2;
    for (int layer = img->layers - 1; layer >= 0; layer--) {
        int current = image_get_pixel(img, layer, row, col);
        result = (current == 2) ? result : current;
    }
    return result;
}

int count_digit_in_layer(Image *img, int layer, int digit) {
    int start = get_layer_start(img, layer);
    int end = get_layer_end(img, layer);
    int count = 0;
    for (int i = start; i < end; i++) {
        if (pixels_get(img->pixels, i) == digit) {
            count++;
        }
    }
    return count;
}

int get_layer_start(Image *img, int layer) {
    return layer * img->width * img->height;
}

int get_layer_end(Image *img, int layer) {
    return get_layer_start(img, layer + 1);
}

Image *image_init(Pixels *pixels, int width, int height) {
    if (!check_format(pixels, width, height)) {
        fprintf(stderr, "ERROR: Image format was not correct - dimensions were (%d, %d) but saw %d pixels.\n", width, height, pixels->count);
        return 0;
    }

    Image *img = malloc(sizeof(Image));
    img->pixels = pixels;
    img->width = width;
    img->height = height;
    img->layers = count_layers(pixels, width, height);
    return img;
}

void image_free(Image *img) {
    free(img);
}

int image_get_pixel(Image *img, int layer, int row, int col) {
    int width = img->width;
    int height = img->height;
    int index = layer * (width * height) + row * width + col;
    return pixels_get(img->pixels, index);
}

bool check_format(Pixels *pixels, int width, int height) {
    return pixels->count % (width * height) == 0;
}

int count_layers(Pixels *pixels, int width, int height) {
    return pixels->count / (width * height);
}

Pixels *pixels_load(FILE *f) {
    Pixels *pixels = pixels_init();
    int pxChar = 0;
    while (pxChar != -1) {
        pxChar = fgetc(f);
        if (pxChar != -1 && pxChar >= 48 && pxChar <= 57) {
            int pixel = pxChar - 48;  // 48 is ASCII '0'
            pixels_append(pixels, pixel);
        }
    }
    return pixels;
}

Pixels *pixels_init() {
    Pixels *pixels = malloc(sizeof(Pixels));
    pixels->count = 0;
    pixels->capacity = INITIAL_PIXELS_CAPACITY;
    pixels->values = malloc(sizeof(int) * pixels->capacity);
    return pixels;
}

void pixels_free(Pixels *pixels) {
    free(pixels->values);
    free(pixels);
}

void pixels_append(Pixels *pixels, int value) {
    if (pixels->count * 2 > pixels->capacity) {
        pixels->capacity *= 2;
        pixels->values = realloc(pixels->values, sizeof(int) * pixels->capacity);
    }

    pixels->values[pixels->count++] = value;
}

int pixels_get(Pixels *pixels, int index) {
    return pixels->values[index];
}
