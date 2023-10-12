/* ********************************************************
 * Copyright ©2019 Lelanthran Manickum, All rights reserved
 * This project  is licensed under the MIT License. See the
 * file LICENSE for more information.
 */


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
   // that are stored within the table WILL NOT be freed; the caller is responsible for freeing
   // any objects that they have allocated.
   //
   // The number of columns specified by ncols is not changeable after the table is created.
   // Every row will have the same number of columns, whether the caller ses them or not. On
   // error or if ncols is specified as zero no table is created and NULL is returned.
   ds_table_t *ds_table_new (size_t ncols);
   void ds_table_del (ds_table_t *table);

   // Get the number of rows and cols in this table
   size_t ds_table_nrows (const ds_table_t *table);
   size_t ds_table_ncols (const ds_table_t *table);

   // Create a new empty row, either at the beginning, the end or at the specified index. When
   // a row is created at a specified index all subsequent rows are shifted down. When index
   // is ot of range the new row is created at the end of the table.
   bool ds_table_row_new (ds_table_t *table, size_t index);
   bool ds_table_row_new_first (ds_table_t *table);
   bool ds_table_row_new_last (ds_table_t *table);

   // Delete a row. When a row is deleted, all subsequent rows are shifted up. Deleting a row
   // does not delete the data objects stored in the row - the caller is responsible for
   // deleting objects that are stored in the table.
   //
   // Deletions of rows that are out of range results in no actions being performed.
   void ds_table_row_del (ds_table_t *table, size_t row);
   void ds_table_row_del_first (ds_table_t *table);
   void ds_table_row_del_last (ds_table_t *table);

   // Set the value at a specific [row,col]. If the specified row and column are out of range
   // no action is performed and the function simply returns.
   void ds_table_set (const ds_table_t *table, size_t row, size_t col, void *el);

   // Get the pointer to the object stored in the table at [row,col]. If the specified row
   // and column are out of range NULL is returned. Note that NULL will also be returned if
   // the data element at the specified [row,col] is NULL.
   void *ds_table_get (const ds_table_t *table, size_t row, size_t col);

   // Iterate  across the table, calling fptr() on each element. The iteration is started
   // at the first row and column, and each row is iterated before proceeding to the next
   // row.
   void ds_table_iterate (const ds_table_t *table, void (*fptr) (void *, size_t r, size_t col));

#ifdef __cpluspls
};
#endif



#endif

