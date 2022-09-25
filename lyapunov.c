#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "lyapunov.h"
#include "pngimage.h"

#define WIDTH		640
#define HEIGHT		480

#define OUTPUT_FILE  "lyapunov.png"

#define ITERATIONS	1000

#define MIN_A 		0	// 0 to 4
#define MAX_A		4

#define MIN_B 		0	// 0 to 4
#define MAX_B		4

#define PALETTE_SIZE WIDTH / 2
Color32* palette = NULL;
int palette_size = 0;

// 0 = A, 1 = B
int sequance[] = { 0, 1 }; // AB
// int sequance[] = { 0, 1, 1, 0 }; // ABBA
// int sequance[] = { 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 }; // BBBBBBAAAAAA
// int sequance[] = { 0, 0, 1, 0, 1 }; // AABAB

#define SEQUENCE_SIZE (sizeof(sequance) / sizeof(sequance[0]))

float map(float value, float minin, float maxin, float minout, float maxout)
{
	return minout + (value - minin) / (maxin - minin) * (maxout - minout);
}

float clamp(float value, float minimum, float maximum)
{
	return fmin(fmax(value, minimum), maximum);
}

float calc_lambda(float A, float B)
{
    float lambda = 0.0;
    float maxLambda = pow(10.0, 5.0);

    int n0 = 10;	// ignore the first iterates (ignore the inital transient behavior)
    float x = 0.5;	// x0;
    float r;
	float rseq[] = { A, B };
	int maxN = ITERATIONS;

    n0 = clamp(fmin(n0, maxN - n0), 1, maxN); // make n0 smaller if maxN is small

	for (int n = 1; n <= maxN && fabsf(lambda) <= maxLambda; n++)
	{
		r = rseq[sequance[n % SEQUENCE_SIZE]];
        x = r * x * (1.0 - x);

        if (n >= n0)
        {
            lambda += log(fabsf(r * (1.0 - 2.0 * x))) / (float)maxN;
        }
    }

    return clamp(lambda, -2, 2);
}

void render_lyapunov(Image* image)
{
	float mi = 1000000;
	float ma = -1000000;

	printf("Generating Lyapunov fractal\n");
	for (int x = 0; x < image->width; ++x)
	{
		printf("%d / %d \r", x, WIDTH);
		fflush(stdout);

		for (int y = 0; y < image->height; ++y)
		{
			float A = map(x, 0, WIDTH - 1, MIN_A, MAX_A);
			float B = map(y, HEIGHT - 1, 0, MIN_B, MAX_B);

			float lambda = calc_lambda(A, B);

			/*
			if (lambda < mi)
			{
				mi = lambda;
			}

			if (lambda > ma)
			{
				ma = lambda;
			}
			*/

			int c = map(lambda, -2, 2, 0, palette_size);
			SETPIXEL(image, x, y, palette[c]);
		}
	}

	// printf("Van %f tot %f \n", mi, ma);
}

int main(void)
{
	Keyframe kfs[] =
	{
		{ 0, COLOR_BLACK },
		{ PALETTE_SIZE / 2, COLOR_YELLOW },
		{ PALETTE_SIZE / 2 + 1, COLOR_BLUE },
		{ PALETTE_SIZE, COLOR_BLACK }
	};
	/*
	Keyframe kfs[] =
	{
		{ 0, COLOR_BLACK },
		{ PALETTE_SIZE / 4, COLOR_RED },
		{ PALETTE_SIZE / 2, COLOR_YELLOW },
		{ PALETTE_SIZE / 2 + 1, COLOR_MAGENTA },
		{ PALETTE_SIZE, COLOR_BLACK }
	};
	*/

	palette = generate_palette(kfs, sizeof(kfs) / sizeof(kfs[0]), &palette_size);

	Image *image = alloc_image(WIDTH, HEIGHT);

	render_lyapunov(image);

	printf("Saving %s\n", OUTPUT_FILE);
	int ret = save_image_as_png(OUTPUT_FILE, image->width, image->height, image->pixels);
	if (ret)
	{
		fprintf(stderr, "Saving image as PNG failed.\n");
	}

	free_image(image);
	image = NULL;

	free_palette(palette);
	palette = NULL;

	return 0;
}
