
#include <stdio.h>
#include <stdlib.h>

#include "ds_array.h"

static void print_string (void *arg)
{
   char *s = arg;
   printf ("->[%s]\n", s);
}

int main (void)
{
   int ret = EXIT_FAILURE;

   static char *elements[] = {
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

   size_t el_len = sizeof elements / sizeof elements[0];

   void **dsa = ds_array_new ();

   if (!dsa) {
      fprintf (stderr, "Failed to create new array object\n");
      goto errorexit;
   }

   for (size_t i=0; i<el_len; i++) {
      // Note: this inserts the pointer, it does not make a copy of it.
      // This means that if elements[i] was allocated by us (the caller)
      // it must be freed by us.
      if (!(ds_array_ins_tail (&dsa, elements[i]))) {
         fprintf (stderr, "Failed to insert tail element [%zu]:[%s]\n",
                          i, elements[i]);
         goto errorexit;
      }
   }

   size_t dsa_len = ds_array_length (dsa);
   for (size_t i=0; i<dsa_len; i++) {
      // Note: Can also simply use "dsa[i]"
      char *string = ds_array_index (dsa, i);
      printf ("[%zu]:[%s]\n", i, string);
   }

   printf ("===================================\n");

   for (size_t i=0; i<el_len; i++) {
      if (!(ds_array_ins_head (&dsa, elements[i]))) {
         fprintf (stderr, "Failed to insert head element [%zu]:[%s]\n",
                          i, elements[i]);
         goto errorexit;
      }
   }

   ds_array_iterate (dsa, print_string);
   printf ("===================================\n");

   for (size_t i=0; i<2; i++) {
      if (!(ds_array_remove_tail (&dsa))) {
         fprintf (stderr, "Failed to remove tail element [%zu]\n", i);
         goto errorexit;
      }
   }

   for (size_t i=0; i<2; i++) {
      if (!(ds_array_remove_head (&dsa))) {
         fprintf (stderr, "Failed to remove head element [%zu]\n", i);
         goto errorexit;
      }
   }

   ds_array_iterate (dsa, print_string);
   printf ("===================================\n");

   dsa_len = ds_array_length (dsa);

   for (size_t i=0; i<dsa_len; i+=3) {
      if (!(ds_array_remove (&dsa, i))) {
         fprintf (stderr, "Failed to remove element [%zu]:[%s]\n",
                          i, (char *)dsa[i]);
         break;
      }
   }

   ds_array_iterate (dsa, print_string);
   printf ("===================================\n");

   char *tmp;

   while ((tmp = ds_array_remove_head (&dsa))) {
      printf ("[%s]\n", tmp);
   }

   ret = EXIT_SUCCESS;

errorexit:

   // Note: This releases all memory used in maintaining the array of
   // strings, it does not delete the strings themselves.
   ds_array_del (dsa);

   return ret;
}

