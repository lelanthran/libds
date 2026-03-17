
#include <stdio.h>
#include <stdlib.h>

#include "ds_kvstore.h"
#include "ds_hmap.h"


int main (void)
{
   static const struct {
      const char *k;    // Key
      const char *v;    // Inserted value
      const char *o;    // Overwrite with
      bool result;      // Expected insertion return
   } inputs[] = {
      { "one",    "ONE",   "ONE",   true     },
      { "two",    NULL,    "",      true     },    // Will unset this
      { "three",  "three", "333",   true     },
      { NULL,     NULL,    NULL,    false    },
   };
   static const size_t ninputs = sizeof inputs / sizeof inputs[0];

   int ret = EXIT_FAILURE;
   ds_kvstore_t *store = NULL;

   if (!(store = ds_kvstore_new (3))) {
      printf ("Failed to create new storage\n");
      goto cleanup;
   }

   for (size_t i=0; i<ninputs; i++) {
      bool ret = ds_kvstore_set (store, inputs[i].k, inputs[i].v);
      if (ret != inputs[i].result) {
         printf ("Failed to insert [%s:%s]\n", inputs[i].k, inputs[i].v);
         goto cleanup;
      }
      bool exists = ds_kvstore_exists (store, inputs[i].k);
      if (!ret && exists) {
         printf ("Key exists when it is not supposed to [%s:%s]\n",
                  inputs[i].k, inputs[i].v);
         goto cleanup;
      }
      if (ret && !exists) {
         printf ("Key does not exist when it should [%s:%s]\n",
                  inputs[i].k, inputs[i].v);
         goto cleanup;
      }
      const char *v = ds_kvstore_get (store, inputs[i].k);
      if (!v && inputs[i].v) {
         printf ("Expected:got [%s:%s]\n", inputs[i].v, v);
         goto cleanup;
      }

      if (inputs[i].v && v && (strcmp (v, inputs[i].v)) != 0) {
         printf ("Incorrect value stored [%s:%s]\n", inputs[i].k, inputs[i].v);
         goto cleanup;
      }
      printf ("Processed: [%s:%s]\n", inputs[i].k, v);
   }

   const char **keys = ds_kvstore_keys (store);
   for (size_t i=0; keys[i]; i++) {
      printf ("Keyslist[%zu]: [%s]\n", i, keys[i]);
   }
   printf ("============================\n");

   ds_kvstore_unset (store, inputs[1].k);
   if (ds_kvstore_exists (store, inputs[1].k)) {
      printf ("Failed to remove [%s] from keystore\n", inputs[1].k);
      goto cleanup;
   }

   keys = ds_kvstore_keys (store);
   for (size_t i=0; keys[i]; i++) {
      printf ("Keyslist[%zu]: [%s]\n", i, keys[i]);
   }
   printf ("============================\n");

   for (size_t i=0; i<ninputs; i++) {
      bool ret = ds_kvstore_set (store, inputs[i].k, inputs[i].o);
      if (ret != inputs[i].result) {
         printf ("Failed to insert [%s:%s]\n", inputs[i].k, inputs[i].o);
         goto cleanup;
      }
      bool exists = ds_kvstore_exists (store, inputs[i].k);
      if (!ret && exists) {
         printf ("Key exists when it is not supposed to [%s:%s]\n",
                  inputs[i].k, inputs[i].o);
         goto cleanup;
      }
      if (ret && !exists) {
         printf ("Key does not exist when it should [%s:%s]\n",
                  inputs[i].k, inputs[i].o);
         goto cleanup;
      }
      const char *v = ds_kvstore_get (store, inputs[i].k);
      if (!v && inputs[i].o) {
         printf ("Expected:got [%s:%s]\n", inputs[i].o, v);
         goto cleanup;
      }

      if (inputs[i].o && v && (strcmp (v, inputs[i].o)) != 0) {
         printf ("Incorrect value stored [%s:%s]\n", inputs[i].k, inputs[i].o);
         goto cleanup;
      }
      printf ("Processed: [%s:%s]\n", inputs[i].k, v);
   }

   ret = EXIT_SUCCESS;
cleanup:

   ds_kvstore_del (store);

   return ret;
}

