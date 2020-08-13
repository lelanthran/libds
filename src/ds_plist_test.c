
#include <stdio.h>
#include <stdlib.h>


#include "ds_plist.h"

int main (void)
{
   int ret = EXIT_FAILURE;

   ds_plist_t *parent_list = ds_plist_new (NULL);
   printf ("Testing the properties list module.\n");

   if (!parent_list) {
      fprintf (stderr, "Failed to create root parent plist\n");
      goto errorexit;
   }

   ret = EXIT_SUCCESS;
errorexit:

   ds_plist_del (parent_list);

   return ret;
}
