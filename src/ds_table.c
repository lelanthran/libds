#include <stdlib.h>

#include "ds_table.h"

struct ds_table_t {
   size_t nrows;
   size_t ncols;
   void ***data;
};

ds_table_t *ds_table_new (void)
{
   ds_table_t *ret = calloc (1, sizeof *ret);
   return ret;
}

void ds_table_del (ds_table_t *table)
{
   if (!table)
      return;

   for (size_t i=0; i<table->nrows; i++) {
      free (table->data[i]);
   }

   free (table->data);
   free (table);
}

