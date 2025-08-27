/* Small example to demonstrate serialising JSON */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "ds_json.h"

#define EPRINTF(...)       do {\
   fprintf (stderr, "%s:%i ", __FILE__, __LINE__);\
   fprintf (stderr, __VA_ARGS__);\
} while (0)

#define FNAME_HAPPY_PATH      "./tests/ds_json/happy-path.json"

#define FNAME_SAD_PATH_1      "./tests"
#define FNAME_SAD_PATH_2      "./does-not-exist"
#define FNAME_SAD_PATH_3      "/root/.bash_history"

static char *fslurp (const char *fname)
{
   bool error = true;
   char *ret = NULL;
   FILE *inf = fopen (fname, "rt");
   size_t fsize = 0, nbytes = 0;

   if (!inf) {
      EPRINTF ("[%s]: Failed to open file for reading: %m\n", fname);
      goto cleanup;
   }
   if ((fseek (inf, 0, SEEK_END)) != 0) {
      EPRINTF ("[%s]: Failed to seek_end: %m\n", fname);
      goto cleanup;
   }

   long tmp = ftell (inf);
   printf ("0xffff = %u\n", 0xffff);
   if (tmp < 0 || tmp > 0xffff) {
      EPRINTF ("[%s]: Failed to determine file size (got %li bytes): %m\n", fname, tmp);
      goto cleanup;
   }

   fsize = tmp;

   if ((fseek (inf, 0, SEEK_SET)) != 0) {
      EPRINTF ("[%s]: Failed to seek_set: %m\n", fname);
      goto cleanup;
   }

   if (!(ret = malloc (fsize + 1))) {
      EPRINTF ("[%s]: Failed to allocate buffer of %zu bytes: %m\n", fname, fsize);
      goto cleanup;
   }

   ret[fsize] = 0;
   if ((nbytes = fread (ret, 1, fsize, inf)) != fsize) {
      EPRINTF ("[%s]: Short read %zu/%zu bytes\n", fname, nbytes, fsize);
      goto cleanup;
   }

   error = false;
cleanup:
   if (inf) fclose (inf);
   if (error) {
      EPRINTF ("[%s]: Fatal errors encountered, returning failure to caller.\n", fname);
      free (ret);
      ret = NULL;
   }
   return ret;
}

int test_fslurp (void)
{
   static const char *sad_paths[] = {
      FNAME_SAD_PATH_1,
      FNAME_SAD_PATH_2,
      FNAME_SAD_PATH_3,
   };

   int ret = EXIT_FAILURE;
   char *fcontents = fslurp (FNAME_HAPPY_PATH);
   if (!fcontents) {
      EPRINTF ("[%s]: Failed to read happy path, aborting\n", FNAME_HAPPY_PATH);
      goto cleanup;
   }

   printf ("====\n%s\n====\n[%s]: %zu bytes\n",
           fcontents, FNAME_HAPPY_PATH, strlen (fcontents));

   free (fcontents); fcontents = NULL;

   size_t nerrs = 0;
   for (size_t i=0; i<sizeof sad_paths / sizeof sad_paths[0]; i++) {
      if ((fcontents = fslurp (sad_paths[i])) != NULL) {
         EPRINTF ("[%s]: fslurp() succeeded incorrectly\n", sad_paths[i]);
         EPRINTF ("====\n%s\n====\n[%s]: %zu bytes\n",
                          fcontents, sad_paths[i], strlen (sad_paths[i]));
         nerrs++;
      }
      free (fcontents);
      fcontents = NULL;
   }
   if (nerrs) {
      EPRINTF ("%zu errors found, aborting\n", nerrs);
      goto cleanup;
   }

   ret = EXIT_SUCCESS;
cleanup:
   free (fcontents);
   return ret;
}

int test_json_string (void)
{
   int ret = EXIT_FAILURE;
   ds_json_t *root = NULL;
   char *test_string = NULL;

   if (!(test_string = fslurp (FNAME_HAPPY_PATH))) {
      EPRINTF ("Failed to read file [%s], aborting\n", FNAME_HAPPY_PATH);
      goto cleanup;
   }

   if (!(root = ds_json_parse_string (test_string))) {
      EPRINTF ("Failed to parse string from [%s]\n", FNAME_HAPPY_PATH);
      goto cleanup;
   }

   ret = EXIT_SUCCESS;

cleanup:
   free (test_string);
   ds_json_del (root);
   return ret;
}

int main (void)
{
   int ret = EXIT_FAILURE;

   static const struct {
      const char *name;
      int (*fptr) (void);
   } tests[] = {
      { "fslurp",          test_fslurp },
      { "json_string",     test_json_string},
   };


   for (size_t i=0; i<sizeof tests  /sizeof tests[0]; i++) {
      if ((tests[i].fptr ()) != EXIT_SUCCESS) {
         EPRINTF ("[%s] test failure\n", tests[i].name);
         goto cleanup;
      } else {
         printf ("[%s] Passed\n", tests[i].name);
      }

   }

   ret = EXIT_SUCCESS;

cleanup:
   return ret;
}

