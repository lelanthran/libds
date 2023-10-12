/* ********************************************************
 * Copyright ©2019 Lelanthran Manickum, All rights reserved
 * This project  is licensed under the MIT License. See the
 * file LICENSE for more information.
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ds_stack.h"

int main (void)
{
   static const char *elements[] = {
      "one",
      "two",
      "three",
      "four",
      "five",
      "six",
   };

   ds_stack_t *st = ds_stack_new ();

   static size_t nelements = sizeof elements / sizeof elements[0];

   printf ("Testing stack\n");

   if (!st) {
      printf ("Failed to create new stack\n");
      goto cleanup;
   }

   for (size_t i=0; i<nelements; i++) {
      if (!(ds_stack_push (st, elements[i]))) {
         printf ("Failed to push element [%zu:%s]\n", i, elements[i]);
      }
   }

   char *s = NULL;

   if ((strcmp ((s = ds_stack_peek (st)), elements[nelements - 1]))!=0) {
      printf ("Peek failure, got [%s] expected [%s]\n",
               s, elements[nelements - 1]);
      goto cleanup;
   }

   size_t i = nelements;
   while ((s = ds_stack_pop (st))!=NULL) {
      printf ("Popped [%zu:%s]\n", i--, s);
   }


cleanup:
   ds_stack_del (st);
   return EXIT_SUCCESS;
}
