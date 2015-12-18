#include <stdio.h>
#include <stdlib.h>
#include <pgm.h>
#include <pgmfilter.h>
#include <errno.h>
#include <string.h>

#define SIXTEENTH 1.0/16.0
float blur_data[3][3] = {{SIXTEENTH*1.0,SIXTEENTH*2.0,SIXTEENTH*1.0},
                         {SIXTEENTH*2.0,SIXTEENTH*4.0,SIXTEENTH*2.0},
                         {SIXTEENTH*1.0,SIXTEENTH*2.0,SIXTEENTH*1.0}};

float sharp_data[3][3] = {{0,-1,0},
                         {-1,5,-1},
                         {0,-1,0}};


int main(int argc, char *argv[])
{
   printf("P2\n");

   if(argc < 4)
   {
      fprintf(stderr, "Not enough arguments given!\n");
      return 1;
   }

   pgm_data_t* data = fread_pgm_data(argv[1]);
   if(!data)
   {
      fprintf(stderr, "Could not read PGM data: %s\n", strerror(errno));
      return 1;
   }

   filter_kernel_t blur_kernel;
   filter_kernel_t sharp_kernel;

   memcpy(blur_kernel.array, blur_data, sizeof(blur_data));
   memcpy(sharp_kernel.array, sharp_data, sizeof(blur_data));

   pgm_data_t* blurred = filter_pgm_data(data, &blur_kernel);
   pgm_data_t* sharpened = filter_pgm_data(data, &sharp_kernel);

   int retval = fwrite_pgm_data(argv[2], blurred);
   if(retval)
   {
      fprintf(stderr, "Could not write PGM data: %s\n", strerror(retval));
   }

   retval = fwrite_pgm_data(argv[3], sharpened);
   if(retval)
   {
      fprintf(stderr, "Could not write PGM data: %s\n", strerror(retval));
   }

   destroy_pgm_data(blurred);
   destroy_pgm_data(sharpened);
   destroy_pgm_data(data);
   return retval;
}

