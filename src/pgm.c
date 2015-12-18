#include "pgm.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

/**
 * @brief Calculates the maximum value of a PGM image.
 * @param data The PGM data.
 * @return The maximum value.
 */
static unsigned int maxval(pgm_data_t* data)
{
	if(!data || data->sx == 0 || data->sy == 0 || data->array == NULL) return 0;

	unsigned int max = 0;
	size_t offset = 0;
	for(int y = 0; y < data->sy; y++)
	{
		for(int x = 0; x < data->sx; x++)
		{
			offset = data->sx*y+x;
			if(data->array[offset] > max)
				max = data->array[offset];
		}
	}
	
	return max;
}

char* serialize_pgm_array(pgm_data_t* data)
{
	if(!data || data->sx == 0 || data->sy == 0 || data->array == NULL) return NULL;
	
	size_t usedsize = 1;
	char* str = NULL;

	size_t max = maxval(data);
	size_t max_line_length = 0;

	// Calculate maximal number length
	char maxval_buf[50]; // 50 chars should be plenty. THAT'S UGLY THOUGH!
	snprintf(maxval_buf, sizeof(maxval_buf), "%d", max);
	max_line_length = (strlen(maxval_buf) + 1) * data->sx + 1;

	// Buffer for writing partial results
	// calculate maximal line length possible
	char* line = malloc(max_line_length);
	char* tmp = malloc(max_line_length);
	
	// Write preamble
	// maxval is ugly and slow since this way this function needs to iterate
	// multiple times over the 2D array.
	// Should be done further down the road.
	usedsize += sprintf(line, "P2\n%d %d\n%d\n", data->sx, data->sy, max);
	str = realloc(str, usedsize);
	strcpy(str, line);
	line[0] = 0;

	for(int y = 0; y < data->sy; y++)
	{
		for(int x = 0; x < data->sx; x++)
		{
			snprintf(tmp, max_line_length, "%s%s%d", line, ((x > 0) ? " " : ""), (unsigned int) data->array[data->sx * y + x]);
			strncpy(line, tmp, max_line_length);
		}
		
		usedsize += strlen(line) + 1;
		str = realloc(str, usedsize);

		strcat(str, line);
		strcat(str, "\n");
		
		// Reset line so it does not
		// hold any string anymore.
		line[0] = 0;
	}

	free(line);
	free(tmp);
	return str;
}

/**
 * @brief Finds the next line that is no comment in a string.
 * @param str The string to scan.
 * @param line A buffer that can hold one line from the string.
 *
 * DEPRECATED, SHOULD NOT BE USED!
 */
static char* find_next_line(char* str, char* line)
{
	if(!str || !line) return NULL;
	line[0] = 0;
	do
	{
		// Read whole string until it reaches the first \n
		sscanf(str, "%[^\n]", line);
		
		// Skip the line and the remaining \n in the string
		str += strlen(line)+1;
	}
	while(line[0] == '#' && *str != 0);
	return str;
}

pgm_data_t* deserialize_pgm_array(const char* str)
{
	if(!str) return NULL;
	
	char* dup = strdup(str);
	char* iter = dup;
	
	size_t max_value_length = 0;
	char* line = malloc(512);
	line[0] = 0;
	
	size_t len = strlen(str);
	
	size_t sx = 0, sy = 0, max = 0;
	char type[5];
	
	// Read preamble	
	// Find next "real" line
	iter = find_next_line(iter, line);
	// Ignore encoding for now.
	
	// Read width and height
	iter = find_next_line(iter, line);
	sscanf(line, "%d %d", &sx, &sy);

	// Read max brightness value
	iter = find_next_line(iter, line);

	// Calculate maximal length of value in line
	sscanf(line, "%d", &max);
	max_value_length = strlen(line);
	
	if(!sx || !sy)
	{
		free(line);
		return NULL;
	}

	pgm_data_t* data = construct_pgm_data(sx, sy);
	if(!data)
	{
		free(line);
		return NULL;
	}

	// Calculate line length
	line = realloc(line, 1 + sx * (max_value_length + 1));
	
	//char section[32];
	for(int y = 0; y < sy && *iter != 0; y++)
	{
		iter = find_next_line(iter, line);
		char* section = strtok(line, " ");
		for(int x = 0; x < sx && section != NULL; x++)
		{
			unsigned int val;
			sscanf(section, "%d", &val);
			data->array[sx*y+x] = val;

			section = strtok(NULL, " ");
		}
	}

	free(line);
	return data;
}

int fwrite_pgm_data(const char* file, pgm_data_t* data)
{
	if(!file || !data) return EINVAL;

	FILE* f = fopen(file, "w");
	if(!f) return errno;

	// Write preamble
	fprintf(f, "P2\n%d %d\n%d\n", data->sx, data->sy, (unsigned char) maxval(data));

	// Write data!
	size_t offset = 0;
	for(size_t y = 0; y < data->sy; y++)
	{
		for (size_t x = 0; x < data->sx; x++)
			fprintf(f, "%s%d ", ((x == 0) ? "" : " "), (unsigned char) data->array[data->sx * y + x]);

		fputc('\n', f);
	}

	fclose(f);
	return 0;
}

/**
 * @brief Moves the file cursor to the start of the next line that is no comment or to EOF.
 * @param f The file to use.
 */
static void find_next_line_in_file(FILE* f)
{
	int character;
	while((character = fgetc(f)) == '#' && !feof(f))
	{
		while(fgetc(f) != '\n' && !feof(f));
	}
	ungetc(character, f);
}

pgm_data_t* fread_pgm_data(const char* file)
{
	pgm_data_t* data = NULL;
	
	FILE* f = fopen(file, "r");
	if(!f)
	{
		return NULL;
	}

	unsigned int w = 0, h = 0;
	unsigned int max;
	char magic[4];

	find_next_line_in_file(f);

	fscanf(f, "%3s\n", magic);
	if(strcmp(magic, "P2"))
	{
		fprintf(stderr, "Error: PGM magic does not match!\n");
		return NULL;
	}

	find_next_line_in_file(f);

	if(fscanf(f, "%d %d %d", &w, &h, &max) != 3)
	{
		fprintf(stderr, "Error: PGM file is invalid or damaged!\n");
		return NULL;
	}

	data = construct_pgm_data(w, h);

	unsigned int value = 0;
	for(int y = 0; y < h; y++)
	{
		find_next_line_in_file(f);

		if(feof(f))
		{
			fclose(f);
			return data;
		}

		for (int x = 0; x < w; x++)
		{
			fscanf(f, "%d", &value);
			data->array[w*y+x] = value;
		}
	}

	fclose(f);
	return data;
}

pgm_data_t* construct_pgm_data(size_t sx, size_t sy)
{
	pgm_data_t* data = (pgm_data_t*) malloc(sizeof(pgm_data_t));
	
	if(!data) return NULL;
	
	data->sx = sx;
	data->sy = sy;
	data->array = (float*) malloc(sizeof(float) * sx * sy);
	return data;
}

void destroy_pgm_data(pgm_data_t* data)
{
	if(!data) return;
	if(data->array) free(data->array);
	free(data);
}
