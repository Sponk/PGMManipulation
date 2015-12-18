#include <stdio.h>
#include <stdlib.h>
#include <pgm.h>
#include <pgmfilter.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
   printf("P3\n");

   if(argc < 6)
   {
      fprintf(stderr, "Not enough arguments given!\n");
      return 1;
   }

   unsigned int kernelsize = 0;
   sscanf(argv[2], "%d", &kernelsize);

   pgm_data_t* data = fread_pgm_data(argv[1]);
   if(!data)
   {
      fprintf(stderr, "Could not read PGM data: %s\n", strerror(errno));
      return 1;
   }

   pgm_data_t* min = apply_non_linear_minimum(data, kernelsize);
   pgm_data_t* max = apply_non_linear_maximum(data, kernelsize);
   pgm_data_t* med = apply_non_linear_median(data, kernelsize);

   int retval = fwrite_pgm_data(argv[3], min);
   if(retval)
   {
      fprintf(stderr, "Could not write PGM data: %s\n", strerror(retval));
   }

   retval = fwrite_pgm_data(argv[4], max);
   if(retval)
   {
      fprintf(stderr, "Could not write PGM data: %s\n", strerror(retval));
   }


   retval = fwrite_pgm_data(argv[5], med);
   if(retval)
   {
      fprintf(stderr, "Could not write PGM data: %s\n", strerror(retval));
   }

   destroy_pgm_data(min);
   destroy_pgm_data(max);
   destroy_pgm_data(med);
   destroy_pgm_data(data);
   return retval;
}

