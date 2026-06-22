
#ifndef H_DS_KVSTORE
#define H_DS_KVSTORE

#include <stdlib.h>
#include <stdbool.h>

typedef struct ds_kvstore_t ds_kvstore_t;

#ifdef __cplusplus
extern "C" {
#endif

   // Create and delete a key/value storage. Keys are stored in a simple
   // hashmap, the parameter `nbuckets`, if non-zero, specifies how
   // many buckets to initialise the hashmap with. If `nbuckets` is zero
   // a default is chosen.
   ds_kvstore_t *ds_kvstore_new (size_t nbuckets);
   void ds_kvstore_del (ds_kvstore_t *store);

   // Adds key `key` to the key/value storage with the value `value`. Returns
   // `true` on success and `false` on failure.
   //
   // If the key exists, the value is replaced with the new value. If the key
   // does not exist it is created. If NULL is specified as the value, an
   // empty string is stored for the value.
   bool ds_kvstore_set (ds_kvstore_t *store, const char *key, const char *value);

   // Removes a key and its value from the storage, if it exists.
   void ds_kvstore_unset (ds_kvstore_t *store, const char *key);

   // Returns true if `key` exists, false otherwise.
   bool ds_kvstore_exists (const ds_kvstore_t *store, const char *key);

   // Returns the value stored for the specified `key`. If the
   // key does not exist, NULL is returned.
   const char *ds_kvstore_get (const ds_kvstore_t *store, const char *key);

   // Returns a NULL-terminated array of the keys in the storage. The caller
   // MUST NOT free the array and MUST NOT free any element of the array.
   const char **ds_kvstore_keys (ds_kvstore_t *store);

#ifdef __cplusplus
};
#endif


#endif


