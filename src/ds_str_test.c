
#include <stdio.h>
#include <stdlib.h>

#include "ds_str.h"

int main (void)
{
   int ret = EXIT_FAILURE;

   char *test_append = NULL;
   char *test_cat = NULL;
   char *test_dup = ds_str_dup ("Test String");
   char *test_printf = NULL;

   if (!(test_dup)) {
      fprintf (stderr, "dup error\n");
      goto errorexit;
   }

   test_cat = ds_str_cat (test_dup, " [dup data 1]", " [dup data 2]", NULL);
   if (!test_cat) {
      fprintf (stderr, "cat error\n");
      goto errorexit;
   }

   const char *more1[] = {
      "one ", "two ", "three "
   };
   const char *more2[] = {
      "ONE ", "TWO ", "THREE "
   };

   for (size_t i=0; i<sizeof more1/sizeof more1[0]; i++) {
      if (!(ds_str_append (&test_append, more1[i], more2[i], NULL))) {
         fprintf (stderr, "append[%zu] failed\n", i);
      }
   }

   float tmpflt = 10.0 / 3.0;
   if (!(ds_str_printf (&test_printf, "[%s], %zu, %f, [%s]\n",
                           "START", (size_t)-1, tmpflt, "END"))) {
      fprintf (stderr, "printf: Failed\n");
      goto errorexit;
   }

   printf ("test_dup:      [%s]\n", test_dup);
   printf ("test_cat:      [%s]\n", test_cat);
   printf ("test_append:   [%s]\n", test_append);
   printf ("test_printf:   [%s]\n", test_printf);

   ret = EXIT_SUCCESS;

errorexit:

   free (test_dup);
   free (test_cat);
   free (test_append);
   free (test_printf);

   return ret;
}

