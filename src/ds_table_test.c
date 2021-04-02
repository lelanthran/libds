
#include <stdlib.h>
#include <stdio.h>

#include "ds_table.h"

#define LOG(...)     do {\
   fprintf (stderr, "[%s:%i] ", __FILE__, __LINE__);\
   fprintf (stderr, __VA_ARGS__);\
} while (0)

int main (void)
{
   int ret = EXIT_FAILURE;

   ds_table_t *table = NULL;

   printf ("Starting ds_table tests (%s)\n", ds_version);

   if (!(table = ds_table_new ())) {
      LOG ("Failed to create new table\n");
      goto errorexit;
   }

   ret = EXIT_SUCCESS;
errorexit:
   ds_table_del (table);

   return ret;
}

