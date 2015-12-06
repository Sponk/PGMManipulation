extern "C" {
#include "pgm.h"
}

#include "testsuite/tests.h"
#include <cstdlib>
#include <cstdio>

TEST(TestPGM, Basic, 0.0f,
     // initialisation
{
	m_data.data.sx = 3;
	m_data.data.sy = 3;
	
	m_data.data.array = (float*) &m_data.array;
},
// cleanup
{
},
// test
{
    char* str = serialize_pgm_array(&m_data.data);
    printf("str: %s\n", str);
},
// data
{
	pgm_data_t data;
	float array[3][3];
}
);