/* ********************************************************
 * Copyright ©2019 Lelanthran Manickum, All rights reserved
 * This project  is licensed under the MIT License. See the
 * file LICENSE for more information.
 */


#ifndef H_DS_LL
#define H_DS_LL

#include <stdlib.h>
#include <stdbool.h>

typedef struct ds_array_t ds_array_t;

// This array stores pointers to objects that must be allocated and freed by the
// caller.  Removing an entry from the array does not free the object stored by the
// caller. The caller must free all objects that they have allocated.
#ifdef __cplusplus
extern "C" {
#endif

   // Create a new empty array
   ds_array_t *ds_array_new (void);

   // Delete an array. Note that the elements of the array will not be deleted.
   // Use `ds_array_fptr` with a function that frees each element (such as
   // `free()`).
   void ds_array_del (ds_array_t *ll);

   // Make a copy of an array. As a ds_array_t only stores pointers to objects,
   // the copy will store pointers as well. IOW, both the copy and the original
   // will point to the same underlying object.
   ds_array_t *ds_array_copy (const ds_array_t *src,
                              size_t from_index, size_t to_index);

   // Return the number of elements store in the array.
   size_t ds_array_length (const ds_array_t *ll);

   // Return element `i` from the array
   void *ds_array_get (const ds_array_t *ll, size_t i);

   // Run the specified function on all elements of the array. The `fptr` must only
   // have a single parameter, the element.
   void ds_array_fptr (ds_array_t *ll, void (*fptr) (void *));
   //
   // Run the specified function on all elements of the array. The `param` will
   // be passed as the second parameter to `fptr`.
   void ds_array_iterate (const ds_array_t *ll,
                          void (*fptr) (void *, void *), void *param);

   // Create a new ds_array_t out of all elements which satisfy predicate. When
   // predicate returns true for an object then that object is added to the new
   // array.
   ds_array_t *ds_array_filter (const ds_array_t *ll,
                                bool (*predicate) (const void *, void *),
                                void *param);

   // Create a new array which contains the results of calling `fptr` on each
   // element in the array.
   ds_array_t *ds_array_map (const ds_array_t *ll,
                             void *(*fptr) (const void *, void *), void *param);

   // Inserts a new element into the array, either at the end or the beginning.
   void *ds_array_ins_tail (ds_array_t *ll, void *el);
   void *ds_array_ins_head (ds_array_t *ll, void *el);

   // Removes an element from the the array, either the first one or the last one.
   void *ds_array_rm_tail (ds_array_t *ll);
   void *ds_array_rm_head (ds_array_t *ll);

   // Removes element at index `index` from the array
   void *ds_array_rm (ds_array_t *ll, size_t index);

   // Coalesces the array.
   void ds_array_shrink_to_fit (ds_array_t *ll);

#ifdef __cplusplus
};
#endif

#endif


