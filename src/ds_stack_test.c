
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ds_stack.h"

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
   };

   ds_stack_t *st = ds_stack_new ();

   static size_t nelements = sizeof elements / sizeof elements[0];

   printf ("Testing stack, %s\n", ds_version);

   if (!st) {
      printf ("Failed to create new stack\n");
      goto cleanup;
   }

   for (size_t i=0; i<nelements; i++) {
      if (!(ds_stack_push (st, elements[i]))) {
         printf ("Error 1. Push Failure [%zu:%s]\n", i, elements[i]);
      }
   }

   char *s = NULL;

   if ((strcmp ((s = ds_stack_peek (st)), elements[nelements - 1]))!=0) {
      printf ("Error: Peek failure, got [%s] expected [%s]\n",
               s, elements[nelements - 1]);
      goto cleanup;
   }

   size_t i = nelements;
   while ((s = ds_stack_pop (st))!=NULL) {
      printf ("Popped [%zu:%s]\n", i--, s);
   }

   for (size_t i=0; i<nelements; i++) {
      if (!(ds_stack_push (st, elements[i]))) {
         printf ("Error 2. Push Failure [%zu:%s]\n", i, elements[i]);
      }
   }

   ds_stack_clear (st);
   if (ds_stack_peek (st)) {
      printf ("Error: expected stack to be empty\n");
      goto cleanup;
   }

   ret = EXIT_SUCCESS;
cleanup:
   ds_stack_del (st);
   printf ("%s\n", ret ? "FAIL" : "PASS");
   return ret;
}
