#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "ds_array.h"


void **ds_array_new (void)
{
   size_t size = 0;
   void **ret = malloc ((sizeof *ret) * (size + 2));
   ret[0] = (uintptr_t)0;
   ret[1] = NULL;
   return &ret[1];
}

void ds_array_del (void **ll)
{
   if (!ll)
      return;

   free (&ll[-1]);
}

void **ds_array_copy (void **src, size_t from_index, size_t to_index)
{
   size_t nitems;
   void **ret = ds_array_new ();
   bool error = true;

   if (!src)
      return NULL;

   nitems = ds_array_length (src);

   for (size_t i=from_index; i>=from_index && i<to_index && i<nitems; i++) {
      if (!(ds_array_ins_tail (&ret, src[i])))
         goto errorexit;
   }

   error = false;

errorexit:

   if (error) {
      ds_array_del (ret);
      ret = NULL;
   }

   return ret;
}


size_t ds_array_length (void **ll)
{
   if (!ll)
      return 0;

   return (uintptr_t)ll[-1];
}

void *ds_array_index (void **ll, size_t i)
{
   if (!ll)
      return NULL;

   size_t len = (uintptr_t)ll[-1];
   if (i >= len)
      return NULL;

   return ll[i];
}

void ds_array_iterate (void **ll, void (*fptr) (void *))
{
   if (!ll || !fptr)
      return;

   for (size_t i=0; ll[i]; i++) {
      fptr (ll[i]);
   }
}

static bool ds_array_grow (void ***ll, size_t nelems)
{
   void **array = (*ll);
   size_t nitems = (uintptr_t)array[-1];
   size_t newsize = nitems + 2 + nelems;

   void **tmp = realloc (&array[-1], newsize * sizeof *tmp);
   if (!tmp)
      return false;

   tmp[0] = (void *)(uintptr_t)(nitems + nelems);
   memset (&tmp[nitems+1], 0, (newsize - nitems - 1) * sizeof *tmp);
   (*ll) = &tmp[1];

   return true;
}

void ds_array_shrink_to_fit (void ***ll)
{
   void **array = (*ll);
   size_t nitems = (uintptr_t)array[-1];
   size_t newsize = nitems + 2;

   void **tmp = realloc (&array[-1], newsize * sizeof *tmp);
   if (!tmp)
      return;

   tmp[nitems + 1] = NULL;
   (*ll) = &tmp[1];
}

void *ds_array_ins_tail (void ***ll, void *el)
{
   if (!ll || !(*ll) || !el)
      return NULL;

   size_t inspos = (uintptr_t)(*ll)[-1];
   if (!(ds_array_grow (ll, 1)))
      return NULL;

   (*ll)[inspos] = el;

   return (*ll)[inspos];
}

void *ds_array_ins_head (void ***ll, void *el)
{
   if (!ll || !(*ll) || !el)
      return NULL;

   size_t endpos = (uintptr_t)(*ll)[-1];
   if (!(ds_array_grow (ll, 1)))
      return NULL;

   memmove (&(*ll)[1], &(*ll)[0], sizeof (void *) * (endpos + 1));

   (*ll)[0] = el;

   return (*ll)[0];
}

void *ds_array_remove_tail (void ***ll)
{
   if(!ll || !(*ll) || !(*ll)[0])
      return NULL;

   size_t nitems = (uintptr_t) (*ll)[-1];
   void *ret = (*ll)[nitems - 1];
   (*ll)[nitems - 1] = NULL;
   (*ll)[-1] = (void *)(uintptr_t)nitems - 1;

   return ret;
}

void *ds_array_remove_head (void ***ll)
{
   return ds_array_remove (ll, 0);
}


void *ds_array_remove (void ***ll, size_t index)
{
   if (!ll || !*ll)
      return NULL;

   size_t len = ds_array_length (*ll);
   if (index > len)
      return NULL;

   void *ret = (*ll)[index];

   memmove (&(*ll)[index], &(*ll)[index + 1],
            (sizeof (void *)) * (len - index));

   return ret;
}
