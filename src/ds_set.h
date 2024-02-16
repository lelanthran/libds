
#ifndef H_DS_SET
#define H_DS_SET

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef int (ds_set_cmp_t) (const void *, const void *);

typedef struct ds_set_t ds_set_t;

#ifdef __cplusplus
extern "C" {
#endif

   // Create and delete a ds_set_t. Note that the deletion does not
   // delete each element stored, only the set_t itself.
   //
   // The parameter `nbuckets` is used as to determine how many buckets
   // to create for the backing hashmap used for the set.
   //
   // The parameter `cmpfunc` is a mandatory function that will be used
   // to compare objects for equality. It must return 0 for equal, -1 for
   // less than and 1 for greater than.
   ds_set_t *ds_set_new (ds_set_cmp_t *cmpfunc, size_t nbuckets);
   void ds_set_del (ds_set_t *set);

   // Returns <0, 0 or >0:
   // <0: Error
   // 0: Item already exists
   // >0: Item added
   int ds_set_add (ds_set_t *set, const void *object, size_t object_length);
   const void *ds_set_remove (ds_set_t *set, const void *object, size_t object_length);
   void *ds_set_find (ds_set_t *set, const void *object, size_t object_length);

   // Get all the entries as an array of void pointers to the objects. The
   // caller must free the return array only, and not each element in the
   // returned array.
   void **ds_set_entries (const ds_set_t *set);

   // Some convenience functions to iterate across all elements stored in the
   // set.
   void ds_set_fptr (ds_set_t *set, void (*fptr) (void *));
   void ds_set_iterate (ds_set_t *set, void (*fptr) (const void *, void *),
                        void *param);
   void **ds_set_map (ds_set_t *set, void *(*fptr) (const void *, void *),
                      void *param);
   ds_set_t *ds_set_filter (const ds_set_t *set,
                            bool (*predicate) (const void *, void *),
                            void *param);




#ifdef __cplusplus
};
#endif


#endif


