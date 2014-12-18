#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
   if (argc != 2) {
      fprintf(stderr, "Usage: %s [test_data]\n", argv[0]);
      exit(1);
   }

   FILE *flog = fopen("receiver_log", "w");


   fclose(flog);

   return 0;
}
