#include <string.h>

#include "ds_ll.h"

struct ds_ll_t {
    void *value;
    ds_ll_t *prev;
    ds_ll_t *next;
};

static ds_ll_t *ds_ll_new (void *value)
{
   ds_ll_t *ret = NULL;

   if (!(ret = malloc (sizeof *ret)))
      return NULL;

   memset (ret, 0, sizeof *ret);
   ret->value = value;

   return ret;
}

void ds_ll_remove (ds_ll_t *node)
{
   if (!node)
      return;

   ds_ll_t *prev = node->prev,
           *next = node->next;

   if (prev) prev->next = next;
   if (next) next->prev = prev;

   free (node);
}

void ds_ll_del_all (ds_ll_t *node)
{
   if (!node)
      return;

   node = ds_ll_first (node);
   while (node) {
      ds_ll_t *tmp = node->next;
      ds_ll_remove (node);
      node = tmp;
   }
}

ds_ll_t *ds_ll_ins_after (ds_ll_t *prev, void *el)
{
   ds_ll_t *ret = ds_ll_new (el);
   if (!ret)
      return NULL;

   if (prev) {
      ds_ll_t *next = prev->next;
      prev->next = ret;
      ret->prev = prev;
      ret->next = next;
   }

   return ret;
}

ds_ll_t *ds_ll_ins_before (ds_ll_t *next, void *el)
{
   ds_ll_t *ret = ds_ll_new (el);
   if (!ret)
      return NULL;

   if (next) {
      ds_ll_t *prev = next->prev;
      next->prev = ret;
      ret->next = next;
      ret->prev = prev;
   }

   return ret;
}

ds_ll_t *ds_ll_ins_tail (ds_ll_t *node, void *el)
{
   if (!node)
      return NULL;

   node = ds_ll_last (node);

   return ds_ll_ins_after (node, el);
}

ds_ll_t *ds_ll_ins_head (ds_ll_t *node, void *el)
{
   if (!node)
      return NULL;

   node = ds_ll_first (node);

   return ds_ll_ins_before (node, el);
}

void *ds_ll_value (ds_ll_t *node)
{
   return node ? node->value : NULL;
}

ds_ll_t *ds_ll_first (ds_ll_t *node)
{
   if (!node)
      return NULL;

   while (node->prev)
      node = node->prev;

   return node;
}

ds_ll_t *ds_ll_last  (ds_ll_t *node)
{
   if (!node)
      return NULL;

   while (node->next)
      node = node->next;

   return node;
}

ds_ll_t *ds_ll_next  (ds_ll_t *node)
{
   return node ? node->next : NULL;
}

ds_ll_t *ds_ll_prev  (ds_ll_t *node)
{
   return node ? node->prev : NULL;
}

ds_ll_t *ds_ll_forward (ds_ll_t *node, size_t index);

ds_ll_t *ds_ll_back (ds_ll_t *node, size_t index);

