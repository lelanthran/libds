#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "ds_array.h"


void **ds_array_new (void)
{
   return calloc (2, sizeof (void **));
}

void ds_array_del (void **ll)
{
   if (!ll)
      return;

   free (ll);
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

   size_t ret = 0;
   for (ret=0; ll[ret]; ret++)
      ;

   return ret;
}

const void *ds_array_index (void **ll, size_t i)
{
   if (!ll)
      return NULL;

   for (size_t count = 0; ll[count]; count++) {
      if (count==i) {
         return ll[i];
      }
   }

   return NULL;
}

void ds_array_iterate (void **ll, void (*fptr) (const void *))
{
   if (!ll || !fptr)
      return;

   for (size_t i=0; ll[i]; i++) {
      fptr (ll[i]);
   }
}


const void *ds_array_ins_tail (void ***ll, const void *el)
{
   size_t nitems = 0;

   if (!ll || !(*ll) || !el)
      return NULL;

   nitems = ds_array_length (*ll);
   size_t newsize = nitems + 2;

   void **tmp = realloc ((*ll), newsize * sizeof (void *));
   if (!tmp)
      return NULL;

   (*ll) = tmp;

   (*ll)[nitems] = el;
   (*ll)[nitems + 1] = 0;

   return (*ll)[nitems];
}

const void *ds_array_ins_head (void ***ll, const void *el)
{
   size_t nitems = 0;

   if (!ll || !(*ll) || !el)
      return NULL;

   nitems = ds_array_length (*ll);
   size_t newsize = nitems + 2;

   void **tmp = realloc ((*ll), newsize * sizeof (void *));
   if (!tmp)
      return NULL;

   (*ll) = tmp;

   memmove (&(*ll)[1], &(*ll)[0], sizeof (void *) * (nitems + 1));

   (*ll)[0] = el;

   return (*ll)[0];
}

const void *ds_array_remove_tail (void ***ll)
{
   if(!ll || !(*ll) || !(*ll)[0])
      return NULL;

   for (size_t i=1; (*ll)[i-1]; i++) {
      if ((*ll)[i]==0) {
         void *ret = (*ll)[i-1];
         (*ll)[i-1] = 0;
         return ret;
      }
   }

   return NULL;

}

const void *ds_array_remove_head (void ***ll)
{
   return ds_array_remove (ll, 0);
}


const void *ds_array_remove (void ***ll, size_t index)
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
