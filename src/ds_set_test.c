
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "ds_set.h"

#define LOG(...)     do {\
   fprintf (stderr, "%s:%i: in `%s`:", __FILE__, __LINE__, __func__);\
   fprintf (stderr, __VA_ARGS__);\
} while (0);

static char *lstrdup (const char *src)
{
   size_t nbytes = strlen (src) + 1;
   char *ret = malloc (nbytes);
   if (!ret) {
      LOG ("Fatail OOM error copying [%s]\n", src);
   }
   return strcpy (ret, src);
}

static void printstring (const void *obj, size_t len, void *param)
{
   const char *s = obj;
   FILE *outf = param;
   fprintf (outf, "[%s:%i %s()] [%s] %zu bytes\n",
         __FILE__, __LINE__, __func__, s, len);
}

static void *upcase (const void *obj, size_t len, void *param)
{
   (void)param;
   const char *s = obj;
   char *ret = lstrdup (s);
   for (size_t i=0; ret && ret[i]; i++) {
      ret[i] = toupper (ret[i]);
   }
   LOG ("Returning [%s] %zu bytes\n", ret, len);
   return ret;
}

static bool predicate (const void *obj, size_t len, void *param)
{
   static const char *pattern = "ve";
   FILE *outf = param;
   const char *s = obj;
   bool ret = ((strstr (s, pattern))) ? true : false;
   (void)len;
   fprintf (outf, "Checking for [%s] in [%s]: %s\n",
            pattern, s, ret ? "true" : "false");
   return ret;
}

static int cmpfunc (const void *lhs, size_t lhs_len, const void *rhs, size_t rhs_len)
{
   const char *s1 = lhs,
              *s2 = rhs;
   (void)lhs_len;
   (void)rhs_len;
   return strcmp (s1, s2);
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

   const char **entries = NULL;
   size_t *lengths = NULL;

   int ret = EXIT_FAILURE;

   ds_set_t *filtered = NULL;

   ds_set_t *set = ds_set_new (cmpfunc, 3);
   if (!set) {
      LOG ("Failed to create new set\n");
      goto cleanup;
   }

   // Add everything
   for (size_t i=0; i<sizeof values/sizeof *values; i++) {
      char *tmp = lstrdup (values[i]);
      int rc = ds_set_add (set, tmp, strlen (tmp));
      if (rc < 0) {
         LOG ("%zu: Failed to add [%s] to set\n", i, tmp);
         free (tmp);
         goto cleanup;
      }
      if (rc == 0 ) {
         LOG ("%zu: Item [%s] already exists, ignoring\n", i, tmp);
         free (tmp);
      }
      if (rc > 0) {
         LOG ("%zu: Added [%s]\n", i, tmp);
      }
   }

   if (!(filtered = ds_set_filter (set, predicate, stdout))) {
      LOG ("Filtering failure\n");
      goto cleanup;
   }

   LOG ("Filtered results\n");
   ds_set_iterate (filtered, printstring, stdout);
   // Remove some entries
   for (size_t i=0; i<sizeof notexists/sizeof notexists[0]; i++) {
      const char *tmp = ds_set_remove (set, notexists[i], strlen (notexists[i]));
      free ((char *)tmp);
   }

   // Entries that weren't removed must be found
   for (size_t i=0; i<sizeof exists/sizeof exists[0]; i++) {
      char *found = ds_set_find (set, exists[i], strlen (exists[i]));
      if (!found) {
         LOG ("%zu: Failed to find [%s] in set\n", i, exists[i]);
         goto cleanup;
      }
   }

   // Entries that were removed must not be found
   for (size_t i=0; i<sizeof notexists/sizeof notexists[0]; i++) {
      char *found = ds_set_find (set, notexists[i], strlen (notexists[i]));
      if (found) {
         LOG ("%zu: Found [%s] in set\n", i, notexists[i]);
         goto cleanup;
      }
   }

   // Iterate on all items
   entries  = (const char **)ds_set_entries (set, &lengths);
   if (!entries) {
      LOG ("Failed to get pointers to all stored entries\n");
      goto cleanup;
   }
   printf ("All entries follow:\n");
   for (size_t i=0; entries[i]; i++) {
      printf ("%zu: [%s] of length [%zu] bytes\n", i, entries[i], lengths[i]);
   }

   for (size_t i=0; entries[i]; i++) {
      char *found = ds_set_find (set, entries[i], strlen (entries[i]));
      if (!found) {
         LOG ("%zu: Failed to find [%s] in set\n", i, entries[i]);
         goto cleanup;
      }
   }
   free (entries);
   entries = NULL;
   free (lengths);
   lengths = NULL;

   // Try some iteration and mapping functions (fptr handles deletes)
   char **uppers = (char **)ds_set_map (set, upcase, NULL);
   if (!uppers) {
      LOG ("Failed to map set elements to uppercase\n");
      goto cleanup;
   }
   for (size_t i=0; uppers[i]; i++) {
      printf ("upcase[%zu]: [%s]\n", i, uppers[i]);
      free (uppers[i]);
   }
   free (uppers);
   uppers = NULL;

   ds_set_iterate (set, printstring, stdout);

   ret = EXIT_SUCCESS;

cleanup:
   free (entries);
   ds_set_fptr (set, free);
   ds_set_del (set);
   ds_set_del (filtered);
   printf ("Returning %i\n", ret);
   return ret;
}
