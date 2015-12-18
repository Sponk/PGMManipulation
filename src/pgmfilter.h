#ifndef PGMFILTER_H
#define PGMFILTER_H

#include "pgm.h"

/**
 * @brief Describes a 3x3 kernel matrix used when filtering images.
 */
typedef struct
{
	float array[3][3];
}filter_kernel_t;

/**
 * @brief Filters a PGM image with a kernel and returns a new image.
 *
 * The new image needs to be destroyed manually after use.
 * @param in The image to filter.
 * @param kernel The kernel to use.
 * @return The filtered image.
 */
pgm_data_t* filter_pgm_data(pgm_data_t* in, filter_kernel_t* kernel);

/**
 * @brief Filters a PGM image with a kernel, destroys the original and returns a new image.
 *
 * The new image needs to be destroyed after use.
 * @param in The image to filter.
 * @param kernel The kernel to use.
 * @return The filtered image.
 */
pgm_data_t* filter_pgm_data_and_destroy(pgm_data_t* in, filter_kernel_t* kernel);

/**
 * @brief Applies an edge detection filter to produce a binary image.
 *
 * This includes several filtering steps:
 * 1) The image is blurred
 *
 * 2) The image is filled with the euklidian norm of the partial
 *    differentials calculated with the sobel operators.
 *
 * 3) The image is filtered using the non-maximum suppression method
 *
 * 4) The image is filtered using tau so that only pixel values greater
 *    or equal tau are represented as 1, all others by 0.
 *
 * The created image needs to be destroyed after use!
 *
 * @param in The image to filter.
 * @param tau The threshold used to create the binary image.
 * @return The new image.
 */
pgm_data_t* apply_canny_filter(pgm_data_t* in, float tau);

/**
 * @brief Filters the image with a non-linear minimum rank filter.
 *
 * Uses kernelsize to determine a quad around each pixel of width = kernelsize and
 * height = kernelsize and colors each quad with the value of the minimal value
 * within that quad.
 *
 * The new image needs to be destroyed after use!
 *
 * @param in The image to filter.
 * @param kernelsize The size of the kernel to use.
 * @return The new image.
 */
pgm_data_t* apply_non_linear_minimum(pgm_data_t* in, int kernelsize);

/**
 * @brief Filters the image with a non-linear maximum rank filter.
 *
 * Uses kernelsize to determine a quad around each pixel of width = kernelsize and
 * height = kernelsize and colors each quad with the value of the maximal value
 * within that quad.
 *
 * The new image needs to be destroyed after use!
 *
 * @param in The image to filter.
 * @param kernelsize The size of the kernel to use.
 * @return The new image.
 */
pgm_data_t* apply_non_linear_maximum(pgm_data_t* in, int kernelsize);

/**
 * @brief Filters the image with a non-linear median rank filter.
 *
 * Uses kernelsize to determine a quad around each pixel of width = kernelsize and
 * height = kernelsize and colors each quad with the value of the median value
 * within that quad.
 *
 * The median value is calculated by sorting all values within the kernel.
 * The value we need is then at sorted_values[num_values/2].
 *
 * The new image needs to be destroyed after use!
 *
 * @param in The image to filter.
 * @param kernelsize The size of the kernel to use.
 * @return The new image.
 */
pgm_data_t* apply_non_linear_median(pgm_data_t* in, int kernelsize);

/**
 * @brief Applies a non linear filter to a PGM image.
 *
 * @param in The image to filter.
 * @param kernelsize The size of the kernel.
 * @param func The function used to determine the value of each kernel.
 */
pgm_data_t* apply_non_linear(pgm_data_t* in, int kernelsize, float (*func)(float* data, int x, int y, int sx, int sy, int kernelsize));

#endif