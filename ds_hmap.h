
#ifndef H_DS_HMAP
#define H_DS_HMAP

/* Hashmap. Note that this data structure does not make copies of the
 * data it is given; instead it merely stores pointers to the data it is
 * given.
 *
 * Keys are copied, so that callers are free to use variables that go out
 * of scope as keys.
 *
 */

typedef struct ds_hmap_t ds_hmap_t;

#ifdef __cplusplus
extern "C" {
#endif

   // Create a new hashmap. The nbuckets specify the number of buckets in
   // the hashmap. Returns NULL on eror, or a hashmap object on success.
   ds_hmap_t *ds_hmap_new (uint16_t nbuckets);

   // Delete a a hashmap. The data being stored is *not* deleted. All
   // other resources associated with the hashmap is deleted.
   ds_hmap_del (ds_hmap_t *hm);

   // Returns the load factor of the hashmap.
   float ds_hmap_load (ds_hmap_t *hm);

   // Returns the last error that was recorded in this hashmap. The caller
   // must not free the error message returned.
   void ds_hmap_lasterr (int *err, const char * const *ermmsg);

   // Set an item in the hashmap. If this key already exists then the data
   // that is mapped to this key is replaced with the new data specified
   // here.
   //
   // The data is not copied (the caller is still responsible for it) but
   // the key is copied (the caller may free the key after this function
   // returns).
   void *ds_hmap_set (ds_hmap_t *hm, void *key,  size_t keylen,
                                     void *data, size_t datalen);

   // Finds an item in the hashmap and returns the data associated with
   // it. The 'data' is allocated and must be freed by the caller. The
   // length of the data is stored in 'datalen'.
   //
   // If the key is found *AND* the data is successfully copied into an
   // allocated buffer, then true is returned, otherwise false is returned
   // and the caller must call _lasterr() to determine what went wrong.
   bool ds_hmap_get (ds_hmap_t *hm, const void *key,  size_t keylen,
                                    void **data,      size_t *datalen);

   void ds_hmap_remove (ds_hmap_t *hm, void *key, size_t keylen);

#ifdef __cplusplus
};
#endif

#endif
