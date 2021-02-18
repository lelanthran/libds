
#ifndef H_DS_LL
#define H_DS_LL

#include <stdlib.h>

typedef struct ds_array_t ds_array_t;

#ifdef __cplusplus
extern "C" {
#endif

   ds_array_t *ds_array_new (void);
   void ds_array_del (ds_array_t *ll);
   ds_array_t *ds_array_copy (const ds_array_t *src, size_t from_index, size_t to_index);

   size_t ds_array_length (const ds_array_t *ll);
   void *ds_array_index (const ds_array_t *ll, size_t i);
   void ds_array_iterate (const ds_array_t *ll, void (*fptr) (void *));

   void *ds_array_ins_tail (ds_array_t *ll, void *el);
   void *ds_array_ins_head (ds_array_t *ll, void *el);

   void *ds_array_remove_tail (ds_array_t *ll);
   void *ds_array_remove_head (ds_array_t *ll);

   void *ds_array_remove (ds_array_t *ll, size_t index);

   void ds_array_shrink_to_fit (ds_array_t *ll);

#ifdef __cplusplus
};
#endif

#endif


