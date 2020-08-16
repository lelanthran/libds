
#include <stdio.h>
#include <stdlib.h>


#include "ds_plist.h"

int main (void)
{
   int ret = EXIT_FAILURE;

   ds_plist_t *parent_list = ds_plist_new (NULL, "root");
   ds_plist_t *child1 = ds_plist_new (parent_list, "child1");
   printf ("Testing the properties list module.\n");

   if (!parent_list) {
      fprintf (stderr, "Failed to create root parent plist\n");
      goto errorexit;
   }

   ds_plist_value_set (child1, "Child1TestName", "Child1TestValue", NULL);
   ds_plist_dump (parent_list, NULL);

   ret = EXIT_SUCCESS;
errorexit:

   ds_plist_del (parent_list);

   return ret;
}
