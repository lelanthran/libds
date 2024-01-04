
#include <stdlib.h>
#include <stdio.h>

#include "ds_set.h"

static uint64_t hashfunc (const void *item)
{
   const char *s = item;
   return 0;
}

int main (void)
{
   static const char *values[] = {
      "one", "two", "three", "four", "five", "six", "seven",
      "two", "four", "six",
      "one", "three", "five",
   };

   static const char *exists[] = {
      "two", "three", "five", "six",
   };

   static const char *notexists[] = {
      "one", "four", "seven",
   };

   int ret = EXIT_FAILURE;

   ds_set_t *set = ds_set_new (hashfunc, 3);
   if (!set) {
      fprintf (stderr, "Failed to create new set\n");
      goto cleanup;
   }

   // Add everything
   for (size_t i=0; i<sizeof values/sizeof *values; i++) {
      if (!(ds_set_add (set, values[i]))) {
         fprintf (stderr, "%zu: Failed to add [%s] to set\n", i, values[i]);
         goto cleanup;
      }
   }

   // Remove some entries
   for (size_t i=0; i<sizeof notexists/sizeof notexists[0]; i++) {
      ds_set_remove (set, notexists[i]);
   }

   // Entries that weren't removed must be found
   for (size_t i=0; i<sizeof exists/sizeof exists[0]; i++) {
      bool found = ds_set_exists (set, exists[i]);
      if (!found) {
         fprintf (stderr, "%zu: Failed to find [%s] in set\n", i, exists[i]);
         goto cleanup;
      }
   }

   // Entries that were removed must not be found
   for (size_t i=0; i<sizeof notexists/sizeof notexists[0]; i++) {
      bool found = ds_set_exists (set, notexists[i]);
      if (found) {
         fprintf (stderr, "%zu: Found [%s] in set\n", i, notexists[i]);
         goto cleanup;
      }
   }

   // Iterate on all items
   const char **entries  = ds_set_entries (set);
   if (!entries) {
      fprintf (stderr, "Failed to get pointers to all stored entries\n");
      goto cleanup;
   }

   for (size_t i=0; entries[i]; i++) {
      bool found = ds_set_exists (set, entries[i]);
      if (!found) {
         fprintf (stderr, "%zu: Failed to find [%s] in set\n", i, entries[i]);
         goto cleanup;
      }
   }

   ret = EXIT_SUCCESS;

cleanup:
   ds_set_del (set);
   return ret;
}

