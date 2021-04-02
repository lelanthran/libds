
#include <stdlib.h>
#include <stdio.h>

#include "ds_table.h"

#define LOG(...)     do {\
   fprintf (stderr, "[%s:%i] ", __FILE__, __LINE__);\
   fprintf (stderr, __VA_ARGS__);\
} while (0)

static void print_table (ds_table_t *table)
{
   size_t nrows = ds_table_nrows (table);
   size_t ncols = ds_table_ncols (table);

   printf ("================================\n");
   for (size_t i=0; i<nrows; i++) {
      printf ("[%zu] ", i);
      for (size_t j=0; j<ncols; j++) {
         const char *s = ds_table_get (table, i, j);
         printf (" [%s]", s);
      }
      printf (" ||\n");
   }
   printf ("================================\n");
}

static void ifunc (void *p, size_t row, size_t col)
{
   const char *s = p;
   printf (" [%zu,%zu] [%s]\n", row, col, s);
}

int main (void)
{
   static const struct {
      char *c1;
      char *c2;
      char *c3;
      char *c4;
      char *c5;
      char *c6;
   } test1[] = {
      { "R0-one", "R0-two", "R0-three", "R0-four", "R0-five", "R0-six" },
      { "R1-one", "R1-two", "R1-three", "R1-four", "R1-five", "R1-six" },
      { "R2-one", "R2-two", "R2-three", "R2-four", "R2-five", "R2-six" },
      { "R3-one", "R3-two", "R3-three", "R3-four", "R3-five", "R3-six" },
      { "R4-one", "R4-two", "R4-three", "R4-four", "R4-five", "R4-six" },
      { "R5-one", "R5-two", "R5-three", "R5-four", "R5-five", "R5-six" },
   };

   int ret = EXIT_FAILURE;

   ds_table_t *table1 = NULL;

   printf ("Starting ds_table tests (%s)\n", ds_version);

   if (!(table1 = ds_table_new (3))) {
      LOG ("Failed to create new table\n");
      goto errorexit;
   }

   for (size_t i=0; i<sizeof test1 / sizeof test1[0]; i++) {
      if (i % 2) {
         if (!(ds_table_row_new_last (table1))) {
            LOG ("Failed to create row %zu\n", i);
            goto errorexit;
         }
      } else {
         if (!(ds_table_row_new_first (table1))) {
            LOG ("Failed to create row %zu\n", i);
            goto errorexit;
         }
      }
      ds_table_set (table1, i, 0, test1[i].c1);
      ds_table_set (table1, i, 0, test1[i].c2);
      ds_table_set (table1, i, 0, test1[i].c3);
      ds_table_set (table1, i, 0, test1[i].c4);
      ds_table_set (table1, i, 0, test1[i].c5);
      ds_table_set (table1, i, 0, test1[i].c6);
   }

   print_table (table1);

   for (size_t i=0; i<sizeof test1 / sizeof test1[0]; i += 2) {
      if (!(ds_table_row_new_last (table1))) {
         LOG ("Failed to create row %zu\n", i);
         goto errorexit;
      }
      ds_table_row_del (table1, i);
      ds_table_row_del (table1, i);
      ds_table_row_del (table1, i);
      ds_table_row_del (table1, i);
      ds_table_row_del (table1, i);
      ds_table_row_del (table1, i);
   }

   print_table (table1);

   ds_table_iterate (table1, ifunc);

   ret = EXIT_SUCCESS;
errorexit:
   ds_table_del (table1);

   return ret;
}

