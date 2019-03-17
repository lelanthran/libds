
#include <stdio.h>
#include <stdlib.h>

#include "ds_str.h"

int main (void)
{
   int ret = EXIT_FAILURE;

   char *test_append = NULL;
   char *test_cat = NULL;
   char *test_dup = ds_strdup ("Test String");

   if (!(test_dup)) {
      fprintf (stderr, "dup error\n");
      goto errorexit;
   }

   test_cat = ds_strcat (test_dup, " [dup data 1]", " [dup data 2]", NULL);
   if (!test_cat) {
      fprintf (stderr, "cat error\n");
      goto errorexit;
   }

   const char *more1[] = {
      "one ", "two", "three"
   };
   const char *more2[] = {
      "ONE ", "TWO", "THREE"
   };

   for (size_t i=0; i<sizeof more1/sizeof more1[0]; i++) {
      if (!(ds_strappend (&test_append, more1[i], more2[i], NULL))) {
         fprintf (stderr, "append[%zu] failed\n", i);
      }
   }

   printf ("test_cat: [%s]\n", test_cat);
   printf ("test_dup: [%s]\n", test_dup);
   printf ("test_append: [%s]\n", test_append);

   ret = EXIT_SUCCESS;

errorexit:

   free (test_dup);
   free (test_cat);
   free (test_append);

   return ret;
}

