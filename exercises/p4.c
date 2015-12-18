#include <stdio.h>
#include <stdlib.h>
#include <pgm.h>
#include <pgmfilter.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
   printf("P4\n");

   if(argc < 3)
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

   pgm_data_t* edges = apply_canny_filter(data, 120);
   int retval = fwrite_pgm_data(argv[2], edges);
   if(retval)
   {
      fprintf(stderr, "Could not write PGM data: %s\n", strerror(retval));
   }
   else
      printf("Success!\n");

   destroy_pgm_data(data);
   return retval;
}

