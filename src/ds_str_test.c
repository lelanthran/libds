
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ds_str.h"

int main (void)
{
   int ret = EXIT_FAILURE;

   char *test_append = NULL;
   char *test_cat = NULL;
   char *test_chsubst = NULL;
   char *test_dup = ds_str_dup ("Test String");
   char *test_printf = NULL;

   char *test_rtrim = NULL;
   char *test_ltrim = NULL;
   char *test_trim = NULL;

   char *test_ertrim = NULL;
   char *test_eltrim = NULL;
   char *test_etrim = NULL;

   char *test_etrim1 = NULL;
   char *test_etrim2 = NULL;
   char *test_etrim3 = NULL;
   char *test_etrim4 = NULL;
   char *test_etrim5 = NULL;

   char *test_strsubst = NULL;

   char *test_substring1 = NULL,
        *test_substring2 = NULL,
        *test_substring3 = NULL,
        *test_substring4 = NULL,
        *test_substring5 = NULL,
        *test_substring6 = NULL;

   /* ******************************************************************* */

   if (!(test_dup)) {
      fprintf (stderr, "dup error\n");
      goto errorexit;
   }

   /* ******************************************************************* */

   test_cat = ds_str_cat (test_dup, " [dup data 1]", " [dup data 2]", NULL);
   if (!test_cat) {
      fprintf (stderr, "cat error\n");
      goto errorexit;
   }

   /* ******************************************************************* */

   printf ("Start ds_str test, [v%s]\n", ds_version);
   /* ******************************************************************* */

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

   /* ******************************************************************* */

   float tmpflt = 10.0 / 3.0;
   if (!(ds_str_printf (&test_printf, "[%s], %zu, %f, [%s]\n",
                           "START", (size_t)-1, tmpflt, "END"))) {
      fprintf (stderr, "printf: Failed\n");
      goto errorexit;
   }

   /* ******************************************************************* */

   if (!(test_chsubst = ds_str_chsubst (test_cat, 'a', 'A',
                                                  'd', 'D',
                                                  0))) {
      fprintf (stderr, "char substitution failed\n");
      goto errorexit;
   }

   /* ******************************************************************* */

   test_rtrim = ds_str_dup ("   \n   Testing the rtrim function   \n   ");
   test_ltrim = ds_str_dup ("   \n   Testing the ltrim function   \n   ");
   test_trim = ds_str_dup ("   \n   Testing the trim function   \n   ");
   test_ertrim = ds_str_dup ("   \n   ");
   test_eltrim = ds_str_dup ("   \n   ");
   test_etrim = ds_str_dup ("  \n   ");
   test_etrim1 = NULL;
   test_etrim2 = ds_str_dup ("");
   test_etrim3 = ds_str_dup ("12345");
   test_etrim4 = ds_str_dup (" 12345");
   test_etrim5 = ds_str_dup (" 12345");
   test_etrim5[3] = 0;

   if (!test_rtrim || !test_ltrim || !test_trim) {
      fprintf (stderr, "Failed to allocate test strings for trim functions\n");
      goto errorexit;
   }

   ds_str_rtrim (test_rtrim);
   ds_str_ltrim (test_ltrim);
   ds_str_trim (test_trim);

   ds_str_rtrim (test_ertrim);
   ds_str_ltrim (test_eltrim);
   ds_str_trim (test_etrim);

   ds_str_ltrim (test_etrim1);
   ds_str_ltrim (test_etrim2);
   ds_str_ltrim (test_etrim3);
   ds_str_ltrim (test_etrim4);
   ds_str_ltrim (test_etrim5);

   /* ******************************************************************* */

   test_strsubst = ds_str_strsubst (
         "The Source String ONE with replacements everyTWOwhere. THREE "
         "Quite a long FOUR string too. ONE also testing ONE multiple "
         "ONE replacements TWO in the TWO string",
               "ONE",   "one",         // Replacement pair
               "TWO",   "two",         // Replacement pair
               "THREE", "three",       // Replacement pair
               "FOUR",  "four",        // Replacement pair
         NULL);

   if (!test_strsubst) {
      fprintf (stderr, "Failure: str_subst()\n");
      goto errorexit;
   }

   size_t len = strlen (test_strsubst);
   test_substring1 = ds_str_substring (test_strsubst, 10, 5);
   test_substring2 = ds_str_substring (test_strsubst, 0, 0);
   test_substring3 = ds_str_substring (test_strsubst, len - 1, 5);
   test_substring4 = ds_str_substring (test_strsubst, len, 5);
   test_substring5 = ds_str_substring (test_strsubst, len + 1, 5);
   test_substring6 = ds_str_substring (test_strsubst, 0, len);


   /* ******************************************************************* */

   printf ("test_dup:      [%s]\n", test_dup);
   printf ("test_cat:      [%s]\n", test_cat);
   printf ("test_chsubst:  [%s]\n", test_chsubst);
   printf ("test_append:   [%s]\n", test_append);
   printf ("test_printf:   [%s]\n", test_printf);

   printf ("test_rtrim:    [%s]\n", test_rtrim);
   printf ("test_ltrim:    [%s]\n", test_ltrim);
   printf ("test_trim:     [%s]\n", test_trim);

   printf ("test_ertrim:   [%s]\n", test_ertrim);
   printf ("test_eltrim:   [%s]\n", test_eltrim);
   printf ("test_etrim:    [%s]\n", test_etrim);

   printf ("test_etrim1:    [%s]\n", test_etrim1);
   printf ("test_etrim2:    [%s]\n", test_etrim2);
   printf ("test_etrim3:    [%s]\n", test_etrim3);
   printf ("test_etrim4:    [%s]\n", test_etrim4);
   printf ("test_etrim5:    [%s]\n", test_etrim5);

   printf ("test_strsubst: [%s]\n", test_strsubst);

   printf ("&test_strsubst[10]:    [%s]\n", &test_strsubst[10]);
   printf ("&test_strsubst[len-1]: [%s]\n", &test_strsubst[len-1]);
   printf ("&test_strsubst[len]:   [0x%02x]\n", test_strsubst[len]);

   printf ("test_substring1 (10, 15):   [%s]\n", test_substring1);
   printf ("test_substring2 (0, 0):     [%s]\n", test_substring2);
   printf ("test_substring3 (%zu, 5):   [%s]\n", len - 1, test_substring3);
   printf ("test_substring4 (%zu, 5):   [%s]\n", len, test_substring4);
   printf ("test_substring5 (%zu, 5):   [%s]\n", len + 1, test_substring5);
   printf ("test_substring6 (%zu, 5):   [%s]\n", len + 1, test_substring6);

   ret = EXIT_SUCCESS;

errorexit:

   free (test_dup);
   free (test_cat);
   free (test_chsubst);
   free (test_append);
   free (test_printf);

   free (test_rtrim);
   free (test_ltrim);
   free (test_trim);

   free (test_ertrim);
   free (test_eltrim);
   free (test_etrim);

   free (test_etrim1);
   free (test_etrim2);
   free (test_etrim3);
   free (test_etrim4);
   free (test_etrim5);

   free (test_strsubst);

   free (test_substring1);
   free (test_substring2);
   free (test_substring3);
   free (test_substring4);
   free (test_substring5);
   free (test_substring6);

   return ret;
}

