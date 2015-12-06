#ifndef __PGM_H
#define __PGM_H

#include <stddef.h>

typedef struct
{
	float* array;
	size_t sx;
	size_t sy;
} pgm_data_t;

pgm_data_t* construct_pgm_data(size_t sx, size_t sy);
void destroy_pgm_data(pgm_data_t* data);

int fwrite_pgm_data(const char* file, pgm_data_t* data);
pgm_data_t* fread_pgm_data(const char* file);

/**
 * @brief Serializes the given 2D array into a valid pgm file.
 * ATTENTION: The return value needs to be free'd!
 * @param data The data to serialize.
 * @return The string with the PGM data.
 */
char* serialize_pgm_array(pgm_data_t* data);

/**
 * @brief Deserializes the given PGM data into a valid 2D float array.
 * ATTENTION: The return value needs to be free'd!
 * @param data The data to deserialize.
 * @return The pgm_data_t generated.
 */
pgm_data_t* deserialize_pgm_array(const char* str);

#endif