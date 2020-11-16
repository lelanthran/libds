
#include <stdio.h>
#include <stdlib.h>


#include "ds_tree.h"

int main (void)
{
   int ret = EXIT_FAILURE;

   ds_tree_t *root = ds_tree_new (NULL);
   if (!root) {
      fprintf (stderr, "Unable to create a new tree\n");
      goto errorexit;
   }

   ret = EXIT_SUCCESS;
errorexit:

   ds_tree_del (root);
   return ret;
}
