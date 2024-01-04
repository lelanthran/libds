
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

   ds_set_t *ds_set_new (ds_set_cmp_t *cmpfunc, size_t nbuckets);
   void ds_set_del (ds_set_t *set);

   const void *ds_set_add (ds_set_t *set, const void *object, size_t object_length);
   void ds_set_remove (ds_set_t *set, const void *object, size_t object_length);
   bool ds_set_exists (ds_set_t *set, const void *object, size_t object_length);

   void **ds_set_entries (ds_set_t *set);


#ifdef __cplusplus
};
#endif


#endif


