
#ifndef H_DS_SET
#define H_DS_SET

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef uint64_t (ds_set_hashfunc_t) (const void *);

typedef struct ds_set_t ds_set_t;

#ifdef __cplusplus
extern "C" {
#endif

   ds_set_t *ds_set_new (ds_set_hashfunc_t *hashfunc, size_t nbuckets);
   void ds_set_del (ds_set_t *set);


#ifdef __cplusplus
};
#endif


#endif


