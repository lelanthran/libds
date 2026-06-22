#include <stdint.h>

#include "ds_hmap.h"
#include "ds_str.h"
#include "ds_kvstore.h"

#define DEFAULT_NBUCKETS      64

struct ds_kvstore_t {
   ds_hmap_t *hmap;
   char **keys;
};

ds_kvstore_t *ds_kvstore_new (size_t nbuckets)
{
   ds_kvstore_t *ret = calloc (1, sizeof *ret);
   if (!ret)
      return NULL;

   if (!nbuckets)
      nbuckets = DEFAULT_NBUCKETS;

   if (!(ret->hmap = ds_hmap_new (nbuckets))) {
      free (ret);
      ret = NULL;
   }

   return ret;
}

void ds_kvstore_del (ds_kvstore_t *store)
{
   if (!store)
      return;

   const char **keys = ds_kvstore_keys (store);
   for (size_t i=0; keys && keys[i]; i++) {
      char *v = (char *)ds_kvstore_get (store, keys[i]);
      free (v);
   }
   ds_hmap_del (store->hmap);

   for (size_t i=0; store->keys && store->keys[i]; i++) {
      free (store->keys[i]);
   }
   free (store->keys);
   free (store);
}


bool ds_kvstore_set (ds_kvstore_t *store, const char *key, const char *value)
{
   char *newvalue = ds_str_dup (value ? value : "");
   char *oldvalue = NULL;

   if (!key || !newvalue) {
      free (newvalue);
      return false;
   }

   ds_hmap_get_str_str (store->hmap, key, &oldvalue);
   free (oldvalue);

   ds_hmap_remove_str (store->hmap, key);
   if (!(ds_hmap_set_str_str (store->hmap, key, newvalue)))
      return false;

   return true;
}


void ds_kvstore_unset (ds_kvstore_t *store, const char *key)
{
   char *oldvalue = NULL;
   ds_hmap_get_str_str (store->hmap, key, &oldvalue);
   free (oldvalue);
   ds_hmap_remove_str (store->hmap, key);
}


bool ds_kvstore_exists (const ds_kvstore_t *store, const char *key)
{
   char *value = NULL;
   if (!key || !(ds_hmap_get_str_str (store->hmap, key, &value)))
      return false;
   return value != NULL;
}


const char *ds_kvstore_get (const ds_kvstore_t *store, const char *key)
{
   char *ret = NULL;
   if (!key || !(ds_hmap_get_str_str (store->hmap, key, &ret)) || !ret)
      return NULL;
   return ret;
}


const char **ds_kvstore_keys (ds_kvstore_t *store)
{
   bool error = true;
   char **keys = NULL;

   size_t nkeys = ds_hmap_keys (store->hmap, (void ***)&keys, NULL);

   for (size_t i=0; store->keys && store->keys[i]; i++) {
      free (store->keys[i]);
   }

   free (store->keys);
   if (!(store->keys = calloc (nkeys + 1, sizeof *store->keys)))
      goto cleanup;

   for (size_t i=0; i < nkeys; i++) {
      if (!(store->keys[i] = ds_str_dup (keys[i])))
         goto cleanup;
   }

   error = false;

cleanup:
   free (keys);
   if (error) {
      free (store->keys);
      store->keys = NULL;
   }
   return (const char **)store->keys;
}

