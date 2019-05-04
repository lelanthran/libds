
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ds_hmap.h"

/*
static void print_stats (ds_hmap_t *hm, const char *msg)
{
   float lf = ds_hmap_load (hm);
   size_t nbuckets = ds_hmap_num_buckets (hm);
   size_t nentries = ds_hmap_num_entries (hm);
   size_t mean_entries = ds_hmap_mean_entries (hm);
   size_t stddev_entries = ds_hmap_stddev_entries (hm);
   size_t min_entries = ds_hmap_min_entries (hm);
   size_t max_entries = ds_hmap_max_entries (hm);

   printf ("[%s]\n", msg);
   printf ("Load factor:                  %.2f\n", lf);
   printf ("Bucket count:                 %zu\n", nbuckets);
   printf ("Entry count:                  %zu\n", nentries);
   printf ("Avg entries/bucket:           %zu\n", mean_entries);
   printf ("Std-dev entries/bucket:       %zu\n", stddev_entries);
   printf ("Min entries/bucket:           %zu\n", min_entries);
   printf ("Max entries/bucket:           %zu\n", max_entries);
   printf ("---------------------------------------------\n");
}
*/

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

   static const size_t ntests = sizeof tests / sizeof tests[0];

   ds_hmap_t *hm = ds_hmap_new (19);

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

   error = false;

errorexit:
   if (error) {
      ds_hmap_error_t errnum = 0;
      const char *errmsg = NULL;
      ds_hmap_lasterr (hm, &errnum, &errmsg);
      fprintf (stderr, "Failed with err %i [%s]\n", errnum, errmsg);
   }

   ds_hmap_del (hm);

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

errorexit:

   return ret;
}

