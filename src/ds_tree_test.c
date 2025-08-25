
#include <stdio.h>
#include <stdlib.h>

#include "ds_tree.h"
#include "ds_str.h"

static void delstring (ds_tree_t *tree)
{
   char *payload = ds_tree_payload_get (tree);
   free (payload);
}

static void printstring (ds_tree_t *tree)
{
   const char *name = ds_tree_name_get (tree);
   const char *payload = ds_tree_payload_get (tree);
   printf ("[%s] = [%s]\n", name, payload);
}


int main (void)
{
   int ret = EXIT_FAILURE;
   static const struct {
      const char *value;
      const char *old_name;
      const char *new_name;
   } values[] = {
      { "value-one",    "old-one",    "new-one"     },
      { "value-two",    "old-two",    "new-two"     },
      { "value-three",  "old-three",  "new-three"   },
      { "value-four",   "old-four",   "new-four"    },
   };
   static const size_t nvalues = sizeof values / sizeof values[0];

   ds_tree_t *root = ds_tree_new (NULL, "root");

   if (!root) {
      fprintf (stderr, "Failed to al;locate root node\n");
      goto cleanup;
   }

   for (size_t i=0; i<nvalues; i++) {
      ds_tree_t *child = ds_tree_new (root, values[i].old_name);
      if (!child) {
         fprintf (stderr, "Failed to allocate child %zu [%s]\n", i, values[i].old_name);
         goto cleanup;
      }
      ds_tree_payload_set (child, ds_str_dup (values[i].value));
      for (size_t j=0; j<nvalues; j++) {
         ds_tree_t *gchild = ds_tree_new (child, values[j].old_name);
         if (!gchild) {
            fprintf (stderr, "Failed to allocate grandchild [%zu:%zu]\n", i, j);
            goto cleanup;
         }
         ds_tree_payload_set (gchild, ds_str_dup (values[j].value));
      }
   }

   ds_tree_visit (root, printstring);

   size_t nchildren = ds_tree_nchildren (root);
   for (size_t i=0; i<nchildren; i++) {
      ds_tree_t *child = ds_tree_nth_child (root, i);
      if (!child) {
         fprintf (stderr, "Failed to retrieve nth child %zu from root\n", i);
         goto cleanup;
      }
      const char *payload = ds_tree_payload_get (child);
      printf ("nth child [%zu]: [%s]\n", i, payload);
      ds_tree_name_set (child, values[i].new_name);
   }

   ds_tree_visit (root, printstring);

   ds_tree_dump (root, NULL, printstring);

   ret = EXIT_SUCCESS;
cleanup:

   ds_tree_visit (root, delstring);
   ds_tree_del (root);

   return ret;
}

