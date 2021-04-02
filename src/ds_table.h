
#ifndef H_DS_TABLE
#define H_DS_TABLE

#include <stdbool.h>

// This tables stores pointers to objects that must be allocated and freed by the caller.
// Removing an entry from the table does not free the object stored by the caller. The caller
// must free all objects that they have allocated.
typedef struct ds_table_t ds_table_t;
typedef struct ds_table_row_t ds_table_row_t;

#ifdef __cplusplus
extern "C" {
#endif

   // Create and delete a ds_table_t data object. Note that the array is deleted but the objects
   // that are stored within the table WILL NOT be freed; the caller is responsible for freeeing
   // any objects that they have allocated.
   ds_table_t *ds_table_new (void);
   void ds_table_del (ds_table_t *table);

   // Get the number of rows and cols in this table
   size_t ds_table_nrows (ds_table_t *table);
   size_t ds_table_ncols (ds_table_t *table);

   // Create and delete a ds_table_row_t data object. Note that removal removes the
   // row but does not delete or free the data objects stored in that row.
   ds_table_row_t *ds_table_row_ins_first (ds_table_t *table);
   ds_table_row_t *ds_table_row_ins_last (ds_table_t *table);

   bool ds_table_row_rm_first (ds_table_row_t *table);
   bool ds_table_row_rm_last (ds_table_row_t *table);

   // Get the pointer to the object stored in the table at [row,col]
   void *ds_table_index (ds_table_t *table, size_t row, size_t col);

   // Iterate row-first across the table, calling fptr() on each element.
   void ds_table_iterate (const ds_table_t *table, void (*fptr) (void *));

#ifdef __cpluspls
};
#endif



#endif

