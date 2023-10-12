/* ********************************************************
 * Copyright ©2019 Lelanthran Manickum, All rights reserved
 * This project  is licensed under the MIT License. See the
 * file LICENSE for more information.
 */


#include <stdlib.h>
#include <string.h>

#include "ds_plist.h"
#include "ds_array.h"
#include "ds_str.h"


/* ************************************************************************ */
struct nvlist_t {
   char *name;
   ds_array_t *array_values;
};

static void lfree (void *p, void *param)
{
   (void)param;
   free (p);
}

static void nvlist_del (struct nvlist_t *nvl, void *param)
{
   (void)param;
   if (nvl) {
      free (nvl->name);
      ds_array_iterate (nvl->array_values, lfree, NULL);
      ds_array_del (nvl->array_values);
      free (nvl);
   }
}

static struct nvlist_t *nvlist_new (const char *name)
{
   struct nvlist_t *ret = calloc (1, sizeof *ret);

   if (ret) {
      ret->name = ds_str_dup (name);
      ret->array_values = ds_array_new ();
   }

   if (!ret || !ret->name || !ret->array_values) {
      nvlist_del (ret, NULL);
      ret = NULL;
   }

   return ret;
}

static bool nvlist_append (struct nvlist_t *nvl, const char *value)
{
   if (!nvl || !value)
      return false;

   size_t nitems = ds_array_length (nvl->array_values);
   for (size_t i=0; i<nitems; i++) {
      const char *item = ds_array_get (nvl->array_values, i);
      if ((strcmp (item, value))==0) {
         return true;
      }
   }

   if (!(ds_array_ins_head (nvl->array_values, ds_str_dup (value))))
      return false;

   return true;
}

/* ************************************************************************ */

struct ds_plist_t {
   char         *name;
   ds_plist_t   *parent;
   ds_array_t   *array_children;
   ds_array_t   *array_elements;
};


ds_plist_t *ds_plist_new (ds_plist_t *parent, const char *name)
{
   bool error = true;
   ds_plist_t *ret = calloc (1, sizeof *ret);
   if (!ret)
      goto errorexit;


   ret->name = ds_str_dup (name);
   ret->array_children = ds_array_new ();
   ret->array_elements = ds_array_new ();

   if (!ret->name || !ret->array_children || !ret->array_elements)
      goto errorexit;

   if (parent) {
      ds_plist_child_add (parent, ret);
   }

   error = false;

errorexit:

   if (error) {
      ds_plist_del (ret, NULL);
      ret = NULL;
   }

   return ret;
}

void ds_plist_del (ds_plist_t *plist, void *param)
{
   (void)param;

   if (!plist)
      return;

   if (plist->parent) {
      ds_plist_child_rm (plist->parent, plist);
   }

   free (plist->name);

   // TODO: Remove all the elements
   ds_array_iterate (plist->array_elements, (void (*) (void *, void *))nvlist_del, NULL);
   ds_array_del (plist->array_elements);

   // TODO: Remove all the children
   ds_array_iterate (plist->array_children, (void (*) (void *, void *))ds_plist_del, NULL);
   ds_array_del (plist->array_children);

   free (plist);
}


bool ds_plist_child_add (ds_plist_t *parent, ds_plist_t *child)
{
   if (!parent || !child)
      return true;

   if (!(ds_array_ins_tail (parent->array_children, child)))
      return false;

   return true;
}

void ds_plist_child_rm (ds_plist_t *parent, ds_plist_t *child)
{
   if (parent || !child)
      return;

   size_t nchildren = ds_array_length (parent->array_children);
   for (size_t i=0; i<nchildren; i++) {
      ds_plist_t *pchild = ds_array_get (parent->array_children, i);
      if (pchild == child) {
         ds_array_rm (parent->array_children, i);
         break;
      }
   }
}

static void plist_dump (ds_plist_t *plist, FILE *outf, size_t indent)
{
#define PRINT_INDENT(x)    do {\
   for (size_t i=0; i<indent; i++) fputc (' ', outf); \
} while (0)

   size_t nchildren = ds_array_length (plist->array_children),
          nelements = ds_array_length (plist->array_elements);
   PRINT_INDENT (indent);
   fprintf (outf, "[%s] %zu elements, %zu children\n",
                  plist->name, nelements, nchildren);

   for (size_t i=0; i<nelements; i++) {
      struct nvlist_t *value = ds_array_get (plist->array_elements, i);
      PRINT_INDENT (indent + 3);
      fprintf (outf, "name [%s]: ", value->name);
      size_t nvalues = ds_array_length (value->array_values);
      for (size_t j=0; j<nvalues; j++) {
         fprintf (outf, "[%s] ", (char *)ds_array_get (value->array_values, j));
      }
      fprintf (outf, "\n");
   }
   for (size_t i=0; i<nchildren; i++) {
      ds_plist_t *child = ds_array_get (plist->array_children, i);
      plist_dump (child, outf, indent + 3);
   }
   // TODO: Children
}

void ds_plist_dump (ds_plist_t *plist, FILE *outf)
{
   if (!outf)
      outf = stdout;

   if (!plist) {
      fprintf (outf, "NULL Plist object\n");
      return;
   }

   plist_dump (plist, outf, 1);
}

bool ds_plist_value_set (ds_plist_t *plist, const char *name, const char *value, ...)
{
   va_list ap;
   va_start (ap, value);
   bool ret = ds_plist_value_vset (plist, name, value, ap);
   va_end (ap);
   return ret;
}

bool ds_plist_value_vset (ds_plist_t *plist, const char *name, const char *value, va_list ap)
{
   bool ret = true;
   while (name && value) {
      ret = ret && ds_plist_value_append (plist, name, value);
      value = va_arg (ap, const char *);
   }
   return ret;
}

static struct nvlist_t *plist_find_values (ds_plist_t *plist, const char *name)
{
   size_t nelements = ds_array_length (plist->array_elements);
   for (size_t i=0; i<nelements; i++) {
      struct nvlist_t *element = ds_array_get (plist->array_elements, i);
      if ((strcmp (element->name, name))==0) {
         return element;
      }
   }

   return NULL;
}

bool ds_plist_value_append (ds_plist_t *plist, const char *name, const char *value)
{
   bool error = true;
   struct nvlist_t *record = NULL;

   if (!plist || !name || !value)
      goto errorexit;

   record = plist_find_values (plist, name);
   if (record) {
      if (!(nvlist_append (record, value)))
         goto errorexit;
      error = false;
      goto errorexit;
   }

   if (!(record = nvlist_new (name)))
      goto errorexit;

   if (!(nvlist_append (record, value))) {
      nvlist_del (record, NULL);
      goto errorexit;
   }

   if (!(ds_array_ins_tail (plist->array_elements, record))) {
      nvlist_del (record, NULL);
      goto errorexit;
   }

   error = false;

errorexit:
   return !error;
}


