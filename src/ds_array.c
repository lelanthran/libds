#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "ds_array.h"


void **ds_array_new (void)
{
   uint8_t *memblock = calloc (1, (sizeof (size_t) + sizeof (void *)));
   void **ret = (void **)&memblock[sizeof (size_t)];
   return ret;
}

void ds_array_del (void **ll)
{
   if (!ll)
      return;

   uint8_t *memblock = (uint8_t *)ll;
   free (&memblock[-(sizeof (size_t))]);
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

   uint8_t *memblock = (uint8_t *)ll;
   return (size_t)memblock[-(sizeof (size_t))];
}

void *ds_array_index (void **ll, size_t i)
{
   if (!ll)
      return NULL;

   uint8_t *memblock = (uint8_t *)ll;
   size_t len = (size_t)memblock[-(sizeof (size_t))];
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
   uint8_t *memblock = (uint8_t *)*ll;
   size_t nitems = (size_t)memblock[-(sizeof (size_t))];
   size_t newsize = (sizeof (size_t)) + ((nitems + nelems + 1) * sizeof (void *));
   size_t oldsize = (sizeof (size_t)) + ((nitems + 1) * sizeof (void *));

   uint8_t *tmp = realloc (&memblock[-(sizeof (size_t))], newsize);
   if (!tmp)
      return false;

   size_t newlen = nitems + nelems;
   memcpy (tmp, &newlen, sizeof newlen);

   (*ll) = (void **)&tmp[(sizeof (size_t))];
   (*ll)[newlen] = NULL;
   (*ll)[newlen-1] = NULL;

   return true;
}

void ds_array_shrink_to_fit (void ***ll)
{
   uint8_t *memblock = (uint8_t *)(*ll);
   size_t nitems = (size_t)memblock[-(sizeof (size_t))];
   size_t newsize = (sizeof (size_t)) + ((nitems + sizeof (void *)) * sizeof (void*));

   uint8_t *tmp = realloc (&memblock[-(sizeof (size_t))], newsize);
   if (!tmp)
      return;

   (*ll) = (void **)&tmp[-sizeof (size_t)];
}

void *ds_array_ins_tail (void ***ll, void *el)
{
   if (!ll || !(*ll) || !el)
      return NULL;

   uint8_t *memblock = (uint8_t *)*ll;

   size_t inspos = (size_t)memblock[-(sizeof (size_t))];
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

   uint8_t *memblock = (uint8_t *)(*ll);
   size_t nitems = (size_t)memblock[-(sizeof (size_t))];
   nitems--;

   void *ret = (*ll)[nitems];
   (*ll)[nitems] = NULL;
   memcpy (&memblock[-(sizeof (size_t))], &nitems, sizeof nitems);

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
   if (index >= len)
      return NULL;

   void *ret = (*ll)[index];

   memmove (&(*ll)[index], &(*ll)[index + 1],
            (sizeof (void *)) * (len - index));

   uint8_t *memblock = (uint8_t *)(*ll);
   len--;
   memcpy (&memblock[-sizeof (size_t)], &len, sizeof len);

   return ret;
}
