/* Small example to demonstrate serialising JSON */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "ds_json.h"
#include "ds_str.h"

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
   if (tmp < 0 || tmp > 0xffff) {
      EPRINTF ("[%s]: Failed to determine file size (got %li bytes): %m\n", fname, tmp);
      goto cleanup;
   }

   fsize = (size_t)tmp;

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

static const char **str_split (const char *src, char delim)
{
   size_t nitems = 1;
   const char **ret = NULL;
   char *start = NULL, *end = NULL;
   char *tmp = ds_str_dup (src);
   if (!tmp) {
      fprintf (stderr, "Failed to allocate temporary string\n");
      return NULL;
   }

   start = tmp;
   while (start) {
      nitems++;
      start++;
      start = strchr (start, delim);
   }
   if (!(ret = calloc (nitems + 1, sizeof *ret))) {
      fprintf (stderr, "Failed to allocate return value array\n");
      free (tmp);
      return NULL;
   }

   start = tmp;
   size_t index = 0;
   while (start) {
      end = strchr (start, delim);
      if (end) {
         *end = 0;
      }
      ret[index++] = ds_str_dup (start);
      start = end;
      if (start)
         start++;
   }
   ret[index] = ds_str_dup (start);

   free (tmp);
   return ret;
}

static void str_array_free (char ***array)
{
   char **a = *array;
   for (size_t i=0; a[i]; i++) {
      free (a[i]);
   }

   free (*array);
   *array = NULL;
}


int test_json_string (void)
{
   int ret = EXIT_FAILURE;
   ds_json_t *obj = NULL;
   char *test_string = NULL;
   char *output = NULL;

   static const char *spaths[] = {
      "numbers/number-1",
      "symbols",
      "arrays/simple[2]",
      "arrays/composite[0]/obj-0",
   };
   static const size_t nspaths = sizeof spaths/sizeof spaths[0];


   if (!(test_string = fslurp (FNAME_HAPPY_PATH))) {
      EPRINTF ("Failed to read file [%s], aborting\n", FNAME_HAPPY_PATH);
      goto cleanup;
   }

   if (!(obj = ds_json_parse_string (FNAME_HAPPY_PATH, test_string))) {
      EPRINTF ("Failed to parse string from [%s]\n", FNAME_HAPPY_PATH);
      goto cleanup;
   }
   if (!(output = ds_json_stringify (obj))) {
      EPRINTF ("Failed to stringify object\n");
      goto cleanup;
   }
   printf ("========\n%s\n=========\n", output);

   for (size_t i=0; i<nspaths; i++) {
      const char **path = str_split (spaths[i], '/');
      const ds_json_t *target = ds_json_object_geta (obj, path);
      char *tmp = ds_json_stringify (target);
      printf ("[%s] => %s\n", spaths[i], tmp);
      free (tmp);
      str_array_free ((char ***)&path);
   }

   ret = EXIT_SUCCESS;

cleanup:
   free (test_string);
   free (output);
   char **messages = ds_json_messages_get();
   size_t nmessages = 0;
   for (size_t i=0; messages && messages[i]; i++) {
      nmessages++;
   }
   printf ("Messages: %zu\n", nmessages);
   if (nmessages)
      ret = EXIT_FAILURE;

   for (size_t i=0; messages && messages[i]; i++) {
      printf ("[%zu]: %s\n", i, messages[i]);
      free (messages[i]);
   }
   free (messages);
   ds_json_messages_clear ();

   ds_json_del (obj);
   return ret;
}

int test_create (void)
{
   static struct {
      const char *name;
      const char *value;
   } test1[] = {
      { "one",      "one (string)"                       },
      { "two",      "2"                                  },
      { "three",    "3.33"                               },
      { "four",     "[ 1, \"two\", 3.4, true, false ]"   },
      { "five",     "true"                               },
   };
   static const size_t ntest1 = sizeof test1 / sizeof test1[0];

   static const char *test2[] = {
      "one (string)",
      "2",
      "3.33",
      "false",
      "[ 1, \"two\", 3.4, true, false ]",
      "true",
   };
   static const size_t ntest2 = sizeof test2 / sizeof test2[0];

   int ret = EXIT_FAILURE;
   ds_json_t *testobj = NULL;
   ds_json_t *testarr = NULL;
   char *obj_str = NULL;
   char *arr_str = NULL;

   if (!(testobj = ds_json_object_new ()) || !(testarr = ds_json_array_new ())) {
      printf ("Failed to construct JSON objects obj/arr: %p/%p\n", testobj, testarr);
      goto cleanup;
   }

   for (size_t i=0; i<ntest1; i++) {
      const char *n = test1[i].name;
      const char *v = test1[i].value;
      if (!(ds_json_object_append (testobj, n, ds_json_parse_value (v)))) {
         printf ("Failed to construct object `%s:%s`\n", n, v);
         goto cleanup;
      }
   }

   for (size_t i=0; i<ntest2; i++) {
      if (!(ds_json_array_append (testarr, ds_json_parse_value (test2[i])))) {
         printf ("Failed to add field [%s] to array\n", test2[i]);
         goto cleanup;
      }
   }

   if (!(obj_str = ds_json_stringify (testobj)) || !(arr_str = ds_json_stringify(testarr))) {
      printf ("Failed to stringify testobj/testarr: %p/%p\n", obj_str, arr_str);
      goto cleanup;
   }

   printf ("testobj: %s\n", obj_str);
   printf ("testarr: %s\n", arr_str);

   ret = EXIT_SUCCESS;
cleanup:
   free (obj_str);
   free (arr_str);
   ds_json_del (testobj);
   ds_json_del (testarr);

   char **messages = ds_json_messages_get();
   size_t nmessages = 0;
   for (size_t i=0; messages && messages[i]; i++) {
      nmessages++;
   }
   printf ("Messages: %zu\n", nmessages);
   for (size_t i=0; messages && messages[i]; i++) {
      printf ("[%zu]: %s\n", i, messages[i]);
      free (messages[i]);
   }
   free (messages);
   ds_json_messages_clear ();
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
      { "json_create",     test_create},
   };


   for (size_t i=0; i<sizeof tests  /sizeof tests[0]; i++) {
      if ((tests[i].fptr ()) != EXIT_SUCCESS) {
         EPRINTF ("[%s] test failure\n", tests[i].name);
         fflush (stdout);
         fflush (stderr);
         goto cleanup;
      }
      printf ("[%s] Passed\n", tests[i].name);
      fflush (stdout);
      fflush (stderr);
   }

   ret = EXIT_SUCCESS;

cleanup:
   return ret;
}

