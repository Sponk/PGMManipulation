#ifndef __PGM_H
#define __PGM_H

#include <stddef.h>


/**
 * @brief Represents a PGM image in memory.
 */
typedef struct
{
	float* array;
	size_t sx;
	size_t sy;
} pgm_data_t;

/**
 * @brief Constructs an empty PGM image with a given width and height.
 * @param sx The width.
 * @param sy The height.
 * @return The new empty image.
 */
pgm_data_t* construct_pgm_data(size_t sx, size_t sy);

/**
 * @brief Destroys a PGM image that was created with construct_pgm_data.
 * @param data The image to free.
 */
void destroy_pgm_data(pgm_data_t* data);

/**
 * @brief Writes a PGM image to a file.
 * @param file The path to the file to save in.
 * @param data The PGM image to write.
 * @return Zero at success, non-zero (errno or EINVAL) on failure.
 */
int fwrite_pgm_data(const char* file, pgm_data_t* data);

/**
 * @brief Reads a PGM file into memory.
 * @param The file to read from.
 * @return The new PGM image or NULL on failure.
 */
pgm_data_t* fread_pgm_data(const char* file);

/**
 * @brief Serializes the given 2D array into a valid pgm file.
 *
 * ATTENTION: The return value needs to be free'd!
 *
 * @param data The data to serialize.
 * @return The string with the PGM data.
 *
 * DEPRECATED, SHOULD NOT BE USED!
 */
char* serialize_pgm_array(pgm_data_t* data);

/**
 * @brief Deserializes the given PGM data into a valid 2D float array.
 *
 * ATTENTION: The return value needs to be free'd!
 *
 * @param data The data to deserialize.
 * @return The pgm_data_t generated.
 *
 * DEPRECATED, SHOULD NOT BE USED!
 */
pgm_data_t* deserialize_pgm_array(const char* str);

#define CLAMP(x, max, min) ((x < min) ? min : ((x > max) ? max : x))
#define MIN(x,y) ((x < y) ? x : y)
#define MAX(x,y) ((x < y) ? y : x)

#endif