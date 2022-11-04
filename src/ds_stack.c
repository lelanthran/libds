
#include "ds_stack.h"

#include "ds_array.h"

struct ds_stack_t {
   ds_array_t *array;
};


ds_stack_t *ds_stack_new (void)
{
   ds_stack_t *ret = calloc (1, sizeof *ret);
   if (!ret) {
      return NULL;
   }

   if (!(ret->array = ds_array_new ())) {
      free (ret);
      return NULL;
   }

   return ret;
}


void ds_stack_del (ds_stack_t *st)
{
   if (!st)
      return;

   ds_array_del (st->array);
   free (st);
}


bool ds_stack_push (ds_stack_t *st, const void *el)
{
   if (!st)
      return false;

   return ds_array_ins_tail (st->array,(void *)el) == NULL ? false : true;
}


void *ds_stack_pop (ds_stack_t *st)
{
   if (!st)
      return NULL;

   return ds_array_rm_tail (st->array);
}


void *ds_stack_peek (ds_stack_t *st)
{
   if (!st)
      return NULL;

   size_t nitems = ds_array_length (st->array);
   return ds_array_get (st->array, nitems - 1);
}


