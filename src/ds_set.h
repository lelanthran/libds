
#ifndef H_DS_SET
#define H_DS_SET

#include <stdbool.h>

typedef struct ds_set_t ds_set_t;

#ifdef __cplusplus
extern "C" {
#endif

   ds_set_t *ds_set_new (void);
   void ds_set_del (ds_set_t *set);


#ifdef __cplusplus
};
#endif


#endif


