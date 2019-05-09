
#include <stdio.h>
#include <stdlib.h>

#include "ds_ll.h"

int main (void)
{
   int ret = EXIT_FAILURE;

   static const char *elements[] = {
      "one",
      "two",
      "three",
      "four",
      "five",
      "six",
      "seven",
      "eight",
      "nine",
      "ten",
   };
   const size_t nelements = sizeof elements / sizeof elements[0];

   ds_ll_t *l1 = ds_ll_ins_after (NULL, "First List");
   ds_ll_t *l2 = ds_ll_ins_after (NULL, "Second List");
   if (!l1 || !l2) {
      fprintf (stderr, "Failed to create new linked list\n");
      goto errorexit;
   }

   for (size_t i=0; i<nelements; i++) {
      ds_ll_t *l1_node = ds_ll_ins_tail (l1, (void *)elements[i]),
              *l2_node = ds_ll_ins_head (l2, (void *)elements[i]);

      if (!l1_node || !l2_node) {
         fprintf (stderr, "Failed to add node to linked list\n");
         goto errorexit;
      }
   }

   printf ("[%s]\n",(char *)ds_ll_value (l1));
   printf ("[%s]\n",(char *)ds_ll_value (l2));
   ret = EXIT_SUCCESS;

   ds_ll_t *tmp = ds_ll_first (l1);
   while (tmp) {
      printf ("[%s]\n", (const char *)ds_ll_value (tmp));
      tmp = ds_ll_next (tmp);
   }
   tmp = ds_ll_first (l2);
   while (tmp) {
      printf ("[%s]\n", (const char *)ds_ll_value (tmp));
      tmp = ds_ll_next (tmp);
   }
errorexit:


   ds_ll_del_all (l1);
   ds_ll_del_all (l2);

   return ret;
}
