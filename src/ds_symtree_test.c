
#include <stdio.h>
#include <stdlib.h>

#include "ds_symtree.h"

int main (void)
{
   int ret = EXIT_FAILURE;
   char *sval = NULL;
   ds_symtree_t *rootnode = ds_symtree_new (NULL, ds_symtree_OBJECT, "unset", NULL);

   if (!rootnode) {
      printf ("Failed to create rootnode\n");
      goto cleanup;
   }

   if (!(ds_symtree_name_set (rootnode, "rootnode"))) {
      printf ("Failed to reset the name for rootnode\n");
      goto cleanup;
   }

   for (size_t i=0; i<6; i++) {
      char name[100];
      char value[100];
      ds_symtree_t *child = NULL;
      enum ds_symtree_type_t type = ds_symtree_STRING;
      snprintf (name, sizeof name, "node %zu", i);

      if (!(i % 2)) {
         type = ds_symtree_SYMBOL;
         snprintf (name, sizeof name, "Node %zu", i);
         snprintf (value, sizeof value, "%zu", i);
      }
      if (i == 3) {
         type = ds_symtree_ARRAY;
      }
      if (i == 4) {
         type = ds_symtree_OBJECT;
      }

      if (!(child = ds_symtree_new (rootnode, type, name, value))) {
         printf ("Failed to create node %zu\n", i);
         goto cleanup;
      }
      if (!(ds_symtree_value_set (child, value))) {
         printf ("Failed to set value on [%s]\n", name);
      }

      if (i == 3 || i == 4) {
         for (size_t j=0; j<3; j++) {
            char name[100];
            char value[100];
            ds_symtree_t *gchild = NULL;
            snprintf (name, sizeof name, "Grandchild %zu/%zu", i, j);
            snprintf (value, sizeof value, "Grandvalue %zu/%zu", i, j);
            if (!(gchild = ds_symtree_new (child, ds_symtree_STRING, name, NULL))) {
               printf ("Failed to create node %s\n", name);
               goto cleanup;
            }
            if (type == ds_symtree_OBJECT) {
               if (!(ds_symtree_value_set (gchild, value))) {
                  printf ("Failed to set value on [%s]\n", name);
                  goto cleanup;
               }
            }
         }
      }

   }

   sval = ds_symtree_2json (rootnode, 0);
   printf ("%s\n", sval);

   ret = EXIT_SUCCESS;
cleanup:
   free (sval);
   ds_symtree_del (&rootnode);
   return ret;
}

