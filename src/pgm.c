#include "pgm.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define ACC_2D(array, x, y, sx) (array[sx*y+x])

static unsigned int maxval(pgm_data_t* data)
{
	if(!data || data->sx == 0 || data->sy == 0) return 0;

	unsigned int max = 0;
	for(int y = 0; y < data->sy; y++)
	{
		for(int x = 0; x < data->sx; x++)
		{
			if(ACC_2D(data->array, x, y, data->sy) > max)
				max = ACC_2D(data->array, x, y, data->sy);
		}
	}
	
	return max;
}

char* serialize_pgm_array(pgm_data_t* data)
{
	if(!data || data->sx == 0 || data->sy == 0) return NULL;
	
	size_t usedsize = 1;
	char* str = NULL;
	
	// Buffer for writing partial results
	// A line has 70 characters at max
	// because the specifications say so :D
	char line[70];
	char tmp[70];
	
	// Write preamble
	// maxval is ugly and slow since this way this function needs to iterate
	// multiple times over the 2D array.
	// Should be done further down the road.
	usedsize += sprintf(line, "P2\n%d %d\n%%s\n", data->sx, data->sy);//, maxval(data));
	str = realloc(str, usedsize);
	
	int max = 0;
	for(int y = 0; y < data->sy; y++)
	{		
		for(int x = 0; x < data->sx; x++)
		{
			//memset(tmp, sizeof(tmp), 0);
			snprintf(tmp, sizeof(tmp), "%s%s%d", line, ((x > 0) ? " " : ""), (unsigned int) ACC_2D(data->array, x, y, data->sy));
			strcpy(line, tmp);
		
			if(ACC_2D(data->array, x, y, data->sy) > max)
				max = ACC_2D(data->array, x, y, data->sy);
		}
		
		usedsize += strlen(line) + 1;
		str = realloc(str, usedsize);
		//snprintf(str, "%s%s\n", str, line);
		strcat(str, line);
		strcat(str, "\n");
		
		// Reset line so it does not
		// hold any string anymore.
		line[0] = 0;
	}
	
	usedsize += snprintf(tmp, sizeof(tmp), "%d", max);
	str = realloc(str, usedsize);
	
	char* dup = malloc(usedsize);
	snprintf(dup, usedsize, str, tmp);
	
	free(str);
	return dup;
}

static const char* find_next_line(const char* str, char* line)
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
	
	const char* iter = str;
	char line[70];
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
	sscanf(line, "%d", &max);
	
	if(!sx || !sy) return NULL;
	printf("sx = %d sy = %d max = %d\n", sx, sy, max);
	
	pgm_data_t* data = construct_pgm_data(sx, sy);
	
	if(!data) return NULL;
	
	//char section[32];
	for(int y = 0; y < sy && *iter != 0; y++)
	{
		iter = find_next_line(iter, line);
		char* section = strtok(line, " ");
		for(int x = 0; x < sx && section != NULL; x++)
		{
			int val;
			sscanf(section, "%d", &val);
			data->array[sx*y+x] = val;

			section = strtok(NULL, " ");
		}
	}
	
	return data;
}

int fwrite_pgm_data(const char* file, pgm_data_t* data)
{
	char* str = serialize_pgm_array(data);
	if(!str) return EINVAL;

	FILE* f = fopen(file, "w");
	if(!f) return errno;
	
	fwrite(str, strlen(str), 1, f);
	
	fclose(f);
	free(str);
	return 0;
}

pgm_data_t* fread_pgm_data(const char* file)
{
	//char* str = serialize_pgm_array(data);
	//if(!str) return EINVAL;

	char* str = NULL;
	pgm_data_t* data = NULL;
	
	FILE* f = fopen(file, "r");
	if(!f) return NULL;
	
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	fseek(f, 0, SEEK_SET);
		
	str = malloc(size+1);
	fread(str, size, 1, f);
	fclose(f);
	
	data = deserialize_pgm_array(str);
	free(str);
	
	if(data == NULL)
		errno = EINVAL;
	
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