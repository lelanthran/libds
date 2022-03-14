
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "ds_str.h"
#include "ds_hmap.h"

static void print_stats (ds_hmap_t *hm, const char *msg)
{
   float lf = ds_hmap_load (hm);
   size_t nbuckets = ds_hmap_num_buckets (hm);
   size_t nentries = ds_hmap_num_entries (hm);
   size_t mean_entries = ds_hmap_mean_entries (hm);
   float stddev_entries = ds_hmap_stddev_entries (hm);
   size_t min_entries = ds_hmap_min_entries (hm);
   size_t max_entries = ds_hmap_max_entries (hm);
   size_t range_entries = ds_hmap_range_entries (hm);

   printf ("[%s]\n", msg);

   ds_hmap_print_freq (hm, msg, stdout);

   printf ("Load factor:                  %.2f\n", lf);
   printf ("Bucket count:                 %zu\n", nbuckets);
   printf ("Entry count:                  %zu\n", nentries);
   printf ("Avg entries/bucket:           %zu\n", mean_entries);
   printf ("Std-dev entries/bucket:       %.2f\n", stddev_entries);
   printf ("Min entries/bucket:           %zu\n", min_entries);
   printf ("Max entries/bucket:           %zu\n", max_entries);
   printf ("Range entries/bucket:         %zu\n", range_entries);
   printf ("---------------------------------------------\n");
}

static void print_kv (const void *key, size_t keylen, void *data, size_t datalen,
                      void *extra_param)
{
   FILE *outf = extra_param;
   const char *name = key;
   const char *value = data;

   fprintf (outf, "[name:%s] = [value:%s]\n", name, value);
}


static bool small_test (void)
{
   bool error = true;

   static const struct {
      const char *key;
      const char *data;
   } tests[] = {
      { "Name....................1", "Value..............................1" },
      { "Name....................2", "Value..............................2" },
      { "Name....................3", "Value..............................3" },
      { "Name....................4", "Value..............................4" },
      { "Name....................5", "Value..............................5" },
      { "Name....................6", "Value..............................6" },
      { "Name....................7", "Value..............................7" },
      { "Name....................8", "Value..............................8" },
      { "Name....................9", "Value..............................9" },
      { "Name...................10", "Value.............................10" },
      { "Name...................11", "Value.............................11" },
      { "Name...................12", "Value.............................12" },
      { "Name...................13", "Value.............................13" },
      { "Name...................14", "Value.............................14" },
      { "Name...................15", "Value.............................15" },
      { "Name...................16", "Value.............................16" },
      { "Name...................17", "Value.............................17" },
      { "Name...................18", "Value.............................18" },
      { "Name...................19", "Value.............................19" },
   };

   static const struct {
      const char *key;
      const char *data;
   } test_o[] = {
      { "Name....................2", "VALUE..............................2" },
      { "Name....................4", "VALUE..............................4" },
      { "Name....................6", "VALUE..............................6" },
      { "Name....................8", "VALUE..............................8" },
      { "Name...................10", "VALUE.............................10" },
      { "Name...................12", "VALUE.............................12" },
      { "Name...................14", "VALUE.............................14" },
      { "Name...................16", "VALUE.............................16" },
      { "Name...................18", "VALUE.............................18" },
   };

   static const char *test_rm[] = {
      "Name....................2",
      "Name....................6",
      "Name...................10",
      "Name...................14",
      "Name...................18",
   };

   static const size_t ntests = sizeof tests / sizeof tests[0];
   static const size_t ntest_o = sizeof test_o / sizeof test_o[0];
   static const size_t ntest_rm = sizeof test_rm / sizeof test_rm[0];

   const char **keys = NULL;
   size_t *keylens = NULL;

   ds_hmap_t *hm = ds_hmap_new (15);

   if (!hm) {
      fprintf (stderr, "Failed to create hashmap\n");
      goto errorexit;
   }

   for (size_t i=0; i<ntests; i++) {
      if (!(ds_hmap_set_str_str (hm, tests[i].key, tests[i].data))) {
         fprintf (stderr, "[%zu] Failed to set [%s:%s]\n", i,
                                                           tests[i].key,
                                                           tests[i].data);
         goto errorexit;
      }
   }

   for (size_t i=0; i<ntests; i+=2) {
      char *data = NULL;
      if (!(ds_hmap_get_str_str (hm, tests[i].key, &data))) {
         fprintf (stderr, "%zu Failed to get [%s]\n", i, tests[i].key);
         goto errorexit;
      }
      printf ("%zu Found [%s:%s]\n", i, tests[i].key, data);
   }

   for (size_t i=0; i<ntest_o; i++) {
      if (!(ds_hmap_set_str_str (hm, test_o[i].key, test_o[i].data))) {
         fprintf (stderr, "[%zu] Failed to set [%s:%s]\n", i,
                                                           test_o[i].key,
                                                           test_o[i].data);
         goto errorexit;
      }
   }

   for (size_t i=0; i<ntest_o; i+=2) {
      char *data = NULL;
      if (!(ds_hmap_get_str_str (hm, test_o[i].key, &data))) {
         fprintf (stderr, "%zu Failed to get [%s]\n", i, test_o[i].key);
         goto errorexit;
      }
      printf ("%zu Found [%s:%s]\n", i, test_o[i].key, data);
   }

   size_t nkeys = ds_hmap_keys (hm, (void ***)&keys, &keylens);
   if (nkeys == 0) {
      fprintf (stderr, "Failed to get the keys and key lengths\n");
      goto errorexit;
   }
   for (size_t i=0; i<nkeys; i++) {
      printf ("%zu [%s:%zu]\n", i, keys[i], keylens[i]);
   }
   free (keys);      keys = NULL;
   free (keylens);   keylens = NULL;

   print_stats (hm, "1. SMALL TEST");


   printf ("******************* Removing elements ******************** \n");
   for (size_t i=0; i<ntest_rm; i++) {
      ds_hmap_remove_str (hm, test_rm[i]);
   }
   nkeys = ds_hmap_keys (hm, (void ***)&keys, &keylens);
   if (nkeys == 0) {
      fprintf (stderr, "Failed to get the keys and key lengths\n");
      goto errorexit;
   }
   for (size_t i=0; i<nkeys; i++) {
      printf ("%zu [%s:%zu]\n", i, keys[i], keylens[i]);
   }
   free (keys);      keys = NULL;
   free (keylens);   keylens = NULL;

   print_stats (hm, "2. SMALL TEST");

   for (size_t i=0; i<ntest_rm; i++) {
      if (!(ds_hmap_set_str_str (hm, test_rm[i], "New Data"))) {
         fprintf (stderr, "[%zu] Failed to set [%s:%s]\n", i,
                                                           test_rm[i],
                                                           "New Data");
         goto errorexit;
      }
   }
   printf ("******************* Removing complete ******************** \n");
   printf ("******************* Testing Iteration ******************** \n");

   ds_hmap_iterate (hm, print_kv, stdout);

   printf ("****************** Iteration Complete ******************** \n");

   nkeys = ds_hmap_keys (hm, (void ***)&keys, &keylens);
   if (nkeys == 0) {
      fprintf (stderr, "Failed to get the keys and key lengths\n");
      goto errorexit;
   }
   for (size_t i=0; i<nkeys; i++) {
      char *data;
      if (!(ds_hmap_get_str_str (hm, keys[i], &data))) {
         fprintf (stderr, "%zu Failed to get key [%s]\n",i, keys[i]);
         goto errorexit;
      }
      printf ("%zu [%s:%s]\n", i, keys[i], data);
   }
   free (keys);      keys = NULL;
   free (keylens);   keylens = NULL;

   error = false;

errorexit:
   if (error) {
      ds_hmap_error_t errnum = 0;
      const char *errmsg = NULL;
      ds_hmap_lasterr (hm, &errnum, &errmsg);
      fprintf (stderr, "Failed with err %i [%s]\n", errnum, errmsg);
   }

   free (keys);
   free (keylens);

   ds_hmap_del (hm);

   return !error;
}

static bool large_test (void)
{
   bool error = true;

   char *string = NULL;
   ds_hmap_t *hm = NULL;

   if (!(hm = ds_hmap_new (32000))) {
      fprintf (stderr, "Failed to allocate hashmap of 32k entries\n");
      goto errorexit;
   }

   printf ("Waiting for input from stdin\n");

   int c = 0;
   while (!feof (stdin) && !ferror (stdin) && (c = fgetc (stdin))!=EOF) {
      if (isspace (c)) {
         if (!string) {
            continue;
         }
         if (!(ds_hmap_set_str_str (hm, string, string))) {
            fprintf (stderr, "Failed to set string [%s]\n", string);
            goto errorexit;
         }
         // fprintf (stderr, "Added [%s]\n", string);
         free (string);
         string = NULL;
         continue;
      }
      char st[2];
      st[0] = (char)c;
      st[1] = 0;
      if (!(ds_str_append (&string, st, NULL))) {
         fprintf (stderr, "Out of memory allocating string\n");
         goto errorexit;
      }
   }

   print_stats (hm, "Large test: results");
   error = false;

errorexit:

   ds_hmap_del (hm);
   free (string);

   return !error;
}

int main (void)
{
   int ret = EXIT_FAILURE;
   ret = EXIT_SUCCESS;

   if (!(small_test ())) {
      fprintf (stderr, "Failed small test\n");
      goto errorexit;
   }

   if (!(large_test ())) {
      fprintf (stderr, "Failed large test\n");
      goto errorexit;
   }

errorexit:

   return ret;
}

