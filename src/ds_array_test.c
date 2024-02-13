/* ********************************************************
 * Copyright ©2019 Lelanthran Manickum, All rights reserved
 * This project  is licensed under the MIT License. See the
 * file LICENSE for more information.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ds_array.h"

#define LOG_MSG(...)       do {\
   printf ("[%s:%i] ", __FILE__, __LINE__);\
   printf (__VA_ARGS__);\
} while (0)

static char *lstrdup (const char *s)
{
   char *ret = NULL;
   size_t len = strlen (s) + 1;
   if (!(ret = malloc (len))) {
      return NULL;
   }
   strcpy (ret, s);
   return ret;
}

static void *upcase (const void *element, void *param)
{
   (void)param;
   char *newval = lstrdup ((const char *)element);
   for (size_t i=0; newval[i]; i++) {
      newval[i] = toupper(newval[i]);
   }
   return newval;
}

static bool match_string (const void *element, void *param)
{
   const char *el = element;
   char *p = param;
   return (strstr (el, p)) ? true : false;
}

static void print_string (void *arg, void *param)
{
   const char *s = arg;
   FILE *of = param;
   fprintf (of, "[%s:%i] ", __FILE__, __LINE__);\
   fprintf (of, "->[%s]\n", s);
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

   ds_array_t *dsa = ds_array_new ();
   ds_array_t *filtered = NULL;
   ds_array_t *mapped = NULL;

   if (!dsa) {
      LOG_MSG ("Failed to create new array object\n");
      goto errorexit;
   }

   for (size_t i=0; i<el_len; i++) {
      // Note: this inserts the pointer, it does not make a copy of it.
      // This means that if elements[i] was allocated by us (the caller)
      // it must be freed by us.
      if (!(ds_array_ins_tail (dsa, elements[i]))) {
         LOG_MSG ("Failed to insert tail element [%zu]:[%s]\n",
                          i, elements[i]);
         goto errorexit;
      }
   }

   LOG_MSG ("test: get ===================================\n");

   size_t dsa_len = ds_array_length (dsa);
   for (size_t i=0; i<dsa_len; i++) {
      // Note: Can also simply use "dsa[i]"
      char *string = ds_array_get (dsa, i);
      LOG_MSG ("[%zu]:[%s]\n", i, string);
   }

   LOG_MSG ("test: insert head ============================\n");
   for (size_t i=0; i<el_len; i++) {
      if (!(ds_array_ins_head (dsa, elements[i]))) {
         LOG_MSG ("Failed to insert head element [%zu]:[%s]\n",
                  i, elements[i]);
         goto errorexit;
      }
   }

   ds_array_iterate (dsa, print_string, stdout);
   LOG_MSG ("===================================\n");

   for (size_t i=0; i<2; i++) {
      if (!(ds_array_rm_tail (dsa))) {
         LOG_MSG ("Failed to remove tail element [%zu]\n", i);
         goto errorexit;
      }
   }

   for (size_t i=0; i<2; i++) {
      if (!(ds_array_rm_head (dsa))) {
         LOG_MSG ("Failed to remove head element [%zu]\n", i);
         goto errorexit;
      }
   }

   ds_array_iterate (dsa, print_string, stdout);
   LOG_MSG ("test: rm_head =====================\n");

   dsa_len = ds_array_length (dsa);

   for (size_t i=0; i<dsa_len; i+=3) {
      if (!(ds_array_rm (dsa, i))) {
         LOG_MSG ("Failed to remove element [%zu]:[%s]\n",
                  i, "TODO");
                          // i, (char *)dsa[i]);
         break;
      }
   }

   ds_array_iterate (dsa, print_string, stdout);
   LOG_MSG ("test: rm ==========================\n");

   if (!(filtered = ds_array_filter (dsa, match_string, "e"))) {
      LOG_MSG ("Failed to filter elements matching 'e'\n");
      goto errorexit;
   }
   ds_array_iterate (filtered, print_string, stdout);
   LOG_MSG ("test: filter ======================\n");

   if (!(mapped = ds_array_map (dsa, upcase, NULL))) {
      LOG_MSG ("Failed to map elements to `upcase()`\n");
      goto errorexit;
   }
   ds_array_iterate (mapped, print_string, stdout);
   LOG_MSG ("test: map =========================\n");


   char *tmp;

   while ((tmp = ds_array_rm_head (dsa))) {
      LOG_MSG ("[%s]\n", tmp);
   }

   LOG_MSG ("test: rm ==========================\n");
   ret = EXIT_SUCCESS;

errorexit:

   // Note: This releases all memory used in maintaining the array of
   // strings, it does not delete the strings themselves.
   ds_array_del (dsa);
   ds_array_del (filtered);
   ds_array_fptr (mapped, free);
   ds_array_del (mapped);

   return ret;
}

