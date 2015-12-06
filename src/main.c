#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pgm.h"

int main(int argc, char **argv)
{
	pgm_data_t data;
	float array[3][3] = {	{1,2,3},
				{4,5,6},
				{7,8,9}};
	
	data.array = (float*) &array;
	data.sx = 3;
	data.sy = 3;
	
	char* str = serialize_pgm_array(&data);
	printf("Got string: \n%s\n", str);
	
	pgm_data_t* testdata = deserialize_pgm_array(str);
	
	char* otherstr = serialize_pgm_array(testdata);	
	fwrite_pgm_data("test.pgm", testdata);
	
	printf("Deserializing gives: %s\n", serialize_pgm_array(fread_pgm_data("test.pgm")));

	
	// FIXME: Should test for NULL first!
	free(str);
	free(otherstr);
	destroy_pgm_data(testdata);
	return 0;
}
