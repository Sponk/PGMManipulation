extern "C" {
#include "pgm.h"
#include "pgmfilter.h"
}

#include <cmath>

#include "testsuite/tests.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <pgmfilter.h>

TEST(SerializeNull, Basic, 0.0f,
     // initialisation
{
},
// cleanup
{
},
// Test
{
	// Wie Edgy!
	ASSERT(serialize_pgm_array(NULL) == NULL);
},
// data
{
}
);

float test_array[3][3] = {{1,2,3},
			 {4,5,6},
			 {7,8,9}};

const char* test_array_str =
	"P2\n"
	"3 3\n"
	"9\n"
	"1 2 3\n"
	"4 5 6\n"
	"7 8 9\n";
			 
			 
TEST(SerializeSimple, Basic, 0.0f,
     // initialisation
{
	m_data.data = construct_pgm_data(3,3);
	memcpy(m_data.data->array, &test_array, sizeof(test_array));
},
// cleanup
{
},
// Test
{
	const char* output = serialize_pgm_array(m_data.data);

	//printf("output: \n%s\n", output);

	ASSERT(output != NULL);
	ASSERT(!strcmp(output, test_array_str));
},
// data
{
	pgm_data_t* data;
}
);



TEST(DeserializeSimple, Basic, 0.0f,
     // initialisation
{
},
// cleanup
{
},
// Test
{
	pgm_data_t* data = deserialize_pgm_array(test_array_str);
	ASSERT(data != NULL); // TODO: Compare to test data!
},
// data
{
}
);

TEST(ReadFileBig, Basic, 0.0f,
     // initialisation
{
},
// cleanup
{
},
// Test
{
	pgm_data_t* data = fread_pgm_data("images/elk.pgm");
	ASSERT(data != NULL);
	destroy_pgm_data(data);
},
// data
{
}
);

TEST(WriteFileBig, Basic, 0.0f,
// initialisation
	 {
	 },
// cleanup
	 {
	 },
// Test
	 {
		 // FIXME: Should not depend on this function!
		 pgm_data_t* data = fread_pgm_data("images/feep.pgm");
		 ASSERT(data != NULL);

		 //const char* str = serialize_pgm_array(data);
		 fwrite_pgm_data("test.pgm", data);

		 destroy_pgm_data(data);

		 //printf("%s\n", str);

	 },
// data
	 {
	 }
);

#define SIXTEENTH 1.0/16.0
float linear_kernel_data[3][3] = {{SIXTEENTH*1.0,SIXTEENTH*2.0,SIXTEENTH*1.0},
								  {SIXTEENTH*2.0,SIXTEENTH*4.0,SIXTEENTH*2.0},
								  {SIXTEENTH*1.0,SIXTEENTH*2.0,SIXTEENTH*1.0}};

TEST(LinearBlurFilter, Basic, 0.0f,
// initialisation
	 {
		 memcpy(m_data.kernel.array, linear_kernel_data, sizeof(linear_kernel_data));
	 },
// cleanup
	 {
	 },
// Test
	 {
		 // FIXME: Should not depend on this function!
		 pgm_data_t* data = fread_pgm_data("images/apples.pgm");
		 ASSERT(data != NULL);

		 pgm_data_t* transformed = filter_pgm_data(data, &m_data.kernel);
		 //const char* str = serialize_pgm_array(data);
		 fwrite_pgm_data("blur.pgmt", transformed);

		 destroy_pgm_data(transformed);
		 destroy_pgm_data(data);

		 //printf("%s\n", str);

	 },
// data
	 {
		 filter_kernel_t kernel;
	 }
);

TEST(CannyFilter, Basic, 0.0f,
// initialisation
	 {
	 },
// cleanup
	 {
	 },
// Test
	 {
		 // FIXME: Should not depend on this function!
		 pgm_data_t* data = fread_pgm_data("images/elk.pgm");
		 ASSERT(data != NULL);

		 pgm_data_t* transformed = apply_canny_filter(data, 120);
		 ASSERT(transformed != NULL);

		 //const char* str = serialize_pgm_array(data);
		 fwrite_pgm_data("canny.pgmt", transformed);

		 destroy_pgm_data(transformed);
		 destroy_pgm_data(data);

		 //printf("%s\n", str);

	 },
// data
	 {
	 }
);

TEST(NonLinearMin, Basic, 0.0f,
// initialisation
	 {
	 },
// cleanup
	 {
	 },
// Test
	 {
		 // FIXME: Should not depend on this function!
		 pgm_data_t* data = fread_pgm_data("images/ski.pgm");
		 ASSERT(data != NULL);

		 pgm_data_t* transformed = apply_non_linear_minimum(data, 5);
		 ASSERT(transformed != NULL);

		 //const char* str = serialize_pgm_array(data);
		 fwrite_pgm_data("nl_min.pgmt", transformed);

		 destroy_pgm_data(transformed);
		 destroy_pgm_data(data);

		 //printf("%s\n", str);

	 },
// data
	 {
	 }
);

TEST(NonLinearMed, Basic, 0.0f,
// initialisation
	 {
	 },
// cleanup
	 {
	 },
// Test
	 {
		 // FIXME: Should not depend on this function!
		 pgm_data_t* data = fread_pgm_data("images/ski.pgm");
		 ASSERT(data != NULL);

		 pgm_data_t* transformed = apply_non_linear_median(data, 7);
		 ASSERT(transformed != NULL);

		 //const char* str = serialize_pgm_array(data);
		 fwrite_pgm_data("nl_med.pgmt", transformed);

		 destroy_pgm_data(transformed);
		 destroy_pgm_data(data);

		 //printf("%s\n", str);

	 },
// data
	 {
	 }
);

TEST(NonLinearMax, Basic, 0.0f,
// initialisation
	 {
	 },
// cleanup
	 {
	 },
// Test
	 {
		 // FIXME: Should not depend on this function!
		 pgm_data_t* data = fread_pgm_data("images/ski.pgm");
		 ASSERT(data != NULL);

		 pgm_data_t* transformed = apply_non_linear_maximum(data, 7);
		 ASSERT(transformed != NULL);

		 //const char* str = serialize_pgm_array(data);
		 fwrite_pgm_data("nl_max.pgmt", transformed);

		 destroy_pgm_data(transformed);
		 destroy_pgm_data(data);

		 //printf("%s\n", str);

	 },
// data
	 {
	 }
);