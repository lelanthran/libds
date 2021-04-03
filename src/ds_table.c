#include <stdlib.h>
#include <string.h>

#include "ds_table.h"

struct ds_table_t {
   size_t nrows;
   size_t ncols;
   void ***data;
};

ds_table_t *ds_table_new (size_t ncols)
{
   if (!ncols)
      return NULL;

   ds_table_t *ret = calloc (1, sizeof *ret);

   if (ret)
      ret->ncols = ncols;

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

size_t ds_table_nrows (const ds_table_t *table)
{
   return table ? table->nrows : 0;
}

size_t ds_table_ncols (const ds_table_t *table)
{
   return table ? table->ncols : 0;
}

bool ds_table_row_new (ds_table_t *table, size_t index)
{
   if (!table)
      return false;

   if (index > table->nrows)
      index = table->nrows;

   size_t newlen = table->nrows + 2;
   void ***tmp = realloc (table->data, newlen * (sizeof *tmp));
   if (!tmp)
      return false;

   tmp[newlen-1] = NULL;
   tmp[newlen-2] = calloc (table->ncols + 1, sizeof *tmp[0]);
   memmove (&tmp[index + 1], &tmp[index],
            (sizeof tmp[0]) * (table->nrows - index));

   table->data = tmp;
   table->nrows++;

   return true;
}

bool ds_table_row_new_first (ds_table_t *table)
{
   return ds_table_row_new (table, 0);
}

bool ds_table_row_new_last (ds_table_t *table)
{
   if (!table)
      return false;
   return ds_table_row_new (table, table->nrows + 1);
}


void ds_table_row_del (ds_table_t *table, size_t row);

#if 0
void ds_table_row_del_first (ds_table_t *table)
{
   ds_table_row_del (table, 0);
}

void ds_table_row_del_last (ds_table_t *table)
{
   ds_table_row_del (table, (size_t)-1);
}
#endif


void ds_table_set (const ds_table_t *table, size_t row, size_t col, void *el)
{
   if (!table || row > table->nrows || col > table->ncols)
      return;

   table->data[row][col] = el;
}

void *ds_table_get (const ds_table_t *table, size_t row, size_t col)
{
   if (!table || row > table->nrows || col > table->ncols)
      return NULL;

   return table->data[row][col];
}

void ds_table_iterate (const ds_table_t *table, void (*fptr) (void *, size_t r, size_t col))
{
   if (!table)
      return;

   for (size_t i=0; i<table->nrows; i++) {
      for (size_t j=0; j<table->ncols; j++) {
         void *el = ds_table_get (table, i, j);
         fptr (el, i, j);
      }
   }
}



























