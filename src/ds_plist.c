
#include <stdlib.h>

#include "ds_plist.h"
#include "ds_array.h"
#include "ds_str.h"


struct ds_plist_t {
   char         *name;
   ds_plist_t   *parent;
   void        **array_children;
   void        **array_elements;
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
      ds_plist_del (ret);
      ret = NULL;
   }

   return ret;
}

void ds_plist_del (ds_plist_t *plist)
{
   if (!plist)
      return;

   if (plist->parent) {
      ds_plist_child_remove (plist->parent, plist);
   }

   free (plist->name);

   // TODO: Remove all the elements
   ds_array_del (plist->array_elements);

   // TODO: Remove all the children
   ds_array_del (plist->array_children);

   free (plist);
}


bool ds_plist_child_add (ds_plist_t *parent, ds_plist_t *child)
{
   if (!parent || !child)
      return true;

   if (!(ds_array_ins_tail (&parent->array_children, child)))
      return false;

   return true;
}

void ds_plist_child_remove (ds_plist_t *parent, ds_plist_t *child)
{
   if (parent || !child)
      return;

   size_t nchildren = ds_array_length (parent->array_children);
   for (size_t i=0; i<nchildren; i++) {
      ds_plist_t *pchild = ds_array_index (parent->array_children, i);
      if (pchild == child) {
         ds_array_remove (&parent->array_children, i);
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
      void **array_values = ds_array_index (plist->array_elements, i);
      size_t nvalues = ds_array_length (array_values);
      PRINT_INDENT (indent + 3);
      fprintf (outf, "name [%s]: ", ds_array_index (array_values, 0));
      for (size_t j=1; j<nvalues; j++) {
         fprintf (outf, "[%s] ", (char *)ds_array_index (array_values, j));
      }
      fprintf (outf, "\n");
   }
   for (size_t i=0; i<nchildren; i++) {
      ds_plist_t *child = ds_array_index (plist->array_children, i);
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

