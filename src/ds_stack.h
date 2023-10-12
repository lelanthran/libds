#ifndef H_DS_STACK
#define H_DS_STACK

typedef struct ds_stack_t ds_stack_t;

#ifdef __cplusplus
extern "C" {
#endif

   ds_stack_t *ds_stack_new (void);
   void ds_stack_del (ds_stack_t *st);

   bool ds_stack_push (ds_stack_t *st, const void *el);
   void *ds_stack_pop (ds_stack_t *ds);
   void *ds_stack_peek (ds_stack_t *ds);

   size_t ds_stack_count (ds_stack_t *st);

#ifdef __cplusplus
};
#endif


#endif


