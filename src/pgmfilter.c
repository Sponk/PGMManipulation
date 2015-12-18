#include "pgmfilter.h"
#include "pgm.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>

static float apply_kernel(float* data, int x, int y, int sx, int sy, filter_kernel_t* kernel)
{
	float sum = 0;
	int idx = 0;
	int idy = 0;

	int max_x = sx - 1;
	int max_y = sy - 1;

	for(int l = -1; l <= 1; l++)
		for(int k = -1; k <= 1; k++)
		{
			// Clamp coordinates into range
			idy = CLAMP(y + l, max_y, 0);
			idx = CLAMP(x + k, max_x, 0);
			
			sum += data[sx * idy + idx] * kernel->array[k + 1][l + 1];
		}

	return sum;
}

static float find_minimum(float* data, int x, int y, int sx, int sy, int kernelsize)
{
	float min = FLT_MAX;
	int idx = 0;
	int idy = 0;

	// We use the range [-kernelsize, kernelsize], so to have a rectangle with width = kernelsize
	// it needs to be halfed.
	kernelsize >>= 1;

	for(int l = -kernelsize; l <= kernelsize; l++)
		for(int k = -kernelsize; k <= kernelsize; k++)
		{
			// Clamp coordinates into range
			idy = (y + l);
			idx = (x + k);

			if(idy < 0 || idy >= sy || idx < 0 || idx >= sx)
				continue;

			//if(idx >= 0 && idx < sx && idy >= 0 && idy < sy)
			min = MIN(min, data[sx * idy + idx]);
		}

	return min;
}

static float find_maximum(float* data, int x, int y, int sx, int sy, int kernelsize)
{
	float max = 0;
	int idx = 0;
	int idy = 0;

	// We use the range [-kernelsize, kernelsize], so to have a rectangle with width = kernelsize
	// it needs to be halfed.
	kernelsize >>= 1;

	for(int l = -kernelsize; l <= kernelsize; l++)
		for(int k = -kernelsize; k <= kernelsize; k++)
		{
			// Clamp coordinates into range
			idy = (y + l);
			idx = (x + k);

			if(idy < 0 || idy >= sy || idx < 0 || idx >= sx)
				continue;

			//if(idx >= 0 && idx < sx && idy >= 0 && idy < sy)
			max = MAX(max, data[sx * idy + idx]);
		}

	return max;
}

int cmp_greater(const void* x, const void* y)
{
	if(*(float*) x <  *(float*) y) return -1;
	if(*(float*) x == *(float*) y) return 0;
	if(*(float*) x >  *(float*) y) return 1;
}

static float find_median(float* data, int x, int y, int sx, int sy, int kernelsize)
{
	int num = (kernelsize * kernelsize);

	// We use the range [-kernelsize, kernelsize], so to have a rectangle with width = kernelsize
	// it needs to be halfed.
	kernelsize >>= 1;

	float* values = (float*) malloc(num * sizeof(float));

	int i = 0;
	int idx = 0;
	int idy = 0;

	for(int l = -kernelsize; l <= kernelsize; l++)
		for(int k = -kernelsize; k <= kernelsize; k++)
		{
			// Clamp coordinates into range
			idy = (y + l);
			idx = (x + k);

			// Skip everything we can't touch
			if(idy < 0 || idy >= sy || idx < 0 || idx >= sx || i >= num)
				continue;

			values[i] = data[sx * idy + idx];
			i++;
		}

	qsort(values, i, sizeof(float), cmp_greater);

	// The median is right in the middle
	float retval = values[num >> 1];
	free(values);

	return retval;
}

pgm_data_t* filter_pgm_data(pgm_data_t* in, filter_kernel_t* kernel)
{
	if(!in || !in->sx || !in->sy || !kernel) return NULL;
	pgm_data_t* result = construct_pgm_data(in->sx, in->sy);

	// Apply the kernel for each pixel
	for(int y = 0; y < in->sy; y++)
	{
		for(int x = 0; x < in->sx; x++)
		{
			result->array[in->sx*y+x] = apply_kernel(in->array, x, y, in->sx, in->sy, kernel);
		}
	}

	return result;
}

pgm_data_t* filter_pgm_data_and_destroy(pgm_data_t* in, filter_kernel_t* kernel)
{
	pgm_data_t* result = filter_pgm_data(in, kernel);
	if(in)
		destroy_pgm_data(in);

	return result;
}

#define SIXTEENTH 1.0/16.0
float canny_blur_kernel[3][3] = {{SIXTEENTH*1.0,SIXTEENTH*2.0,SIXTEENTH*1.0},
								  {SIXTEENTH*2.0,SIXTEENTH*4.0,SIXTEENTH*2.0},
								  {SIXTEENTH*1.0,SIXTEENTH*2.0,SIXTEENTH*1.0}};

float sobel_x[3][3] = {
		{1, 0, -1},
		{2, 0, -2},
		{1,0,-1}};

float sobel_y[3][3] = {
		{1, 2, 1},
		{0, 0, 0},
		{-1,-2,-1}};

pgm_data_t* apply_canny_filter(pgm_data_t* in, float tau)
{
	if(!in) return NULL;

	// Blur image
	// FIXME: The struct layout allows giving the kernel as a float array. Should a real kernel object be used?
	pgm_data_t* result = filter_pgm_data(in, canny_blur_kernel);

	pgm_data_t* euklid_image = construct_pgm_data(in->sx, in->sy);
	pgm_data_t* gradients = construct_pgm_data(in->sx, in->sy);

	// The offset into the 2D array
	size_t offset = 0;

	// Partial differentials
	float px = 0;
	float py = 0;

	for(size_t x = 0; x < in->sx; x++)
		for(size_t y = 0; y < in->sy; y++)
		{
			offset = in->sx * y + x;
			px = apply_kernel(result->array, x, y, in->sx, in->sy, sobel_x);
			py = apply_kernel(result->array, x, y, in->sx, in->sy, sobel_y);

			// Calculate gradient if possible
			if(py > 0)
				gradients->array[offset] = atan(px / py);
			else
				gradients->array[offset] = 0.0f;

			// Calculate euklid norm
			euklid_image->array[offset] = sqrt(px * px + py * py);
		}

	for(size_t x = 0; x < in->sx; x++)
		for(size_t y = 0; y < in->sy; y++)
		{
			offset = in->sx * y + x;
			float grad = gradients->array[offset];
			float pix_value = euklid_image->array[offset];

			// North-South
			if((grad > 67.5 && grad <= 90) || (grad >= -90 && grad <= -67.5))
			{
				if(euklid_image->array[in->sx * CLAMP(y + 1, in->sy, 0) + x] > pix_value
				   || euklid_image->array[in->sx * CLAMP(y - 1, in->sy, 0) + x] > pix_value)
					pix_value = 0;
			}
			else if((grad > -22.5 && grad <= 22.5)) // East-West
			{
				if(euklid_image->array[in->sx * y + CLAMP(x + 1, in->sx, 0)] > pix_value
				   || euklid_image->array[in->sx * CLAMP(y - 1, in->sy, 0) + CLAMP(x - 1, in->sx, 0)] > pix_value)
					pix_value = 0;
			}
			else if((grad > 22.5 && grad <= 67.5)) // North-West South-East
			{
				if(euklid_image->array[in->sx * CLAMP(y + 1, in->sy, 0) + CLAMP(x - 1, in->sx, 0)] > pix_value
				   || euklid_image->array[in->sx * CLAMP(y - 1, in->sy, 0) + CLAMP(x + 1, in->sx, 0)] > pix_value)
					pix_value = 0;
			}
			else if((grad > -27.5 && grad <= -22.5)) // North-Ost South-West
			{
				if(euklid_image->array[in->sx * CLAMP(y + 1, in->sy, 0) + CLAMP(x + 1, in->sx, 0)] > pix_value
				   || euklid_image->array[in->sx * CLAMP(y - 1, in->sy, 0) + CLAMP(x - 1, in->sx, 0)] > pix_value)
					pix_value = 0;
			}

			// Generate binary image
			result->array[offset] = pix_value >= tau;
		}

	// Cleanup
	destroy_pgm_data(euklid_image);
	destroy_pgm_data(gradients);

	return result;
}

pgm_data_t* apply_non_linear(pgm_data_t* in, int kernelsize, float (*func)(float* data, int x, int y, int sx, int sy, int kernelsize))
{
	if(!in || kernelsize == 0 || func == 0) return NULL;

	pgm_data_t* result = construct_pgm_data(in->sx, in->sy);
	size_t offset;
	int last_x = in->sx - 1;
	int last_y = in->sy - 1;

	for(size_t x = 0; x < in->sx; x++)
		for(size_t y = 0; y < in->sy; y++)
		{
			offset = in->sx * y + x;
			result->array[offset] = func(in->array, x, y, in->sx, in->sy, kernelsize);
		}

	return result;
}

pgm_data_t* apply_non_linear_minimum(pgm_data_t* in, int kernelsize)
{
	return apply_non_linear(in, kernelsize, find_minimum);
}

pgm_data_t* apply_non_linear_maximum(pgm_data_t* in, int kernelsize)
{
	return apply_non_linear(in, kernelsize, find_maximum);
}

pgm_data_t* apply_non_linear_median(pgm_data_t* in, int kernelsize)
{
	return apply_non_linear(in, kernelsize, find_median);
}