
#ifndef H_DS_HMAP
#define H_DS_HMAP

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define LOCAL_INLINE

#ifdef __GNUC__
#undef LOCAL_INLINE
#define LOCAL_INLINE __inline__
#endif

/* Hashmap. Note that this data structure does not make copies of the
 * data it is given; instead it merely stores pointers to the data it is
 * given.
 *
 * Keys are copied, so that callers are free to use variables that go out
 * of scope as keys.
 *
 */

typedef enum {
   ds_hmap_ENONE     = 0,
   ds_hmap_ENULLOBJ  = 1,
   ds_hmap_EOOM      = 2,
   ds_hmap_ENOTFOUND = 3,
   ds_hmap_EBADPARAM = 4,
} ds_hmap_error_t;

typedef struct ds_hmap_t ds_hmap_t;

#ifdef __cplusplus
extern "C" {
#endif

   // Create a new hashmap. The nbuckets specify the number of buckets in
   // the hashmap. Returns NULL on error, or a hashmap object on success.
   ds_hmap_t *ds_hmap_new (size_t nbuckets);

   // Delete a a hashmap. The data being stored is *not* deleted. All
   // other resources associated with the hashmap is deleted.
   void ds_hmap_del (ds_hmap_t *hm);

   // Returns the last error that was recorded in this hashmap. The caller
   // must not free the error message returned.
   void ds_hmap_lasterr (ds_hmap_t *hm,
                         ds_hmap_error_t *err, const char **errmsg);

   // Set an item in the hashmap. If this key already exists then the data
   // that is mapped to this key is replaced with the new data specified
   // here.
   //
   // The data is not copied (the caller is still responsible for it) but
   // the key is copied (the caller may free the key after this function
   // returns).
   //
   // The length of the data is not used; it is simply stored and returned
   // in a call to ds_hmap_get().
   //
   // On error NULL is returned. On success a pointer to the key is
   // returned.
   const void *ds_hmap_set (ds_hmap_t *hm, const void *key,  size_t keylen,
                                           void *data, size_t datalen);

   // Finds an item in the hashmap and returns the data associated with
   // it. The 'data' is NOT allocated; it is the raw pointer that was
   // passed by the caller during the _set() operation.
   //
   // If the key is found then the 'data' and 'datalen' parameters are
   // populated with a pointer to the data and the length of the data, and
   // true is returned.
   // If the key is not found then the values of 'data' and 'datalen' are
   // indeterminate and false is returned.
   bool ds_hmap_get (ds_hmap_t *hm, const void *key,  size_t keylen,
                                    void **data,      size_t *datalen);

   // Iterate across the hashmap in an unspecified order and call fptr() for each
   // key/value pair.
   void ds_hmap_iterate (ds_hmap_t *hm, void (*fptr) (const void *key, size_t keylen,
                                                      void *value, size_t value_len,
                                                      void *extra_param),
                                        void *extra_param);

   // Removes an item from the hashmap. The data stored in the value field
   // still remains the responsibility of the caller.
   void ds_hmap_remove (ds_hmap_t *hm, const void *key, size_t keylen);

   // Allocates and returns arrays of all the keys and their respective
   // lengths in the arrays provided by the caller. On success, the length
   // of these arrays are returned (both arrays have to be the same
   // length). On error zero is returned.
   //
   // If 'keys' is not NULL then it is populated with an array of pointers
   // to all the keys stored in the hashmap. If 'keylens' is not NULL then
   // is it populated with an array of all the lengths of keys in the
   // hashmap.
   //
   // The caller is responsible for freeing the array stored at 'keys' and
   // the array stored at 'keylens' after this function returns. Note that
   // only the array must be freed, and not each element of the array.
   size_t ds_hmap_keys (ds_hmap_t *hm, void ***keys, size_t **keylens);

   /* These functions all return statistics about the hashmap */

   // Return the load factor of the hashmap.
   float ds_hmap_load (ds_hmap_t *hm);

   // Return the bucket size of the hashmap
   size_t ds_hmap_num_buckets (ds_hmap_t *hm);

   // Return the number of entries in the hashmap
   size_t ds_hmap_num_entries (ds_hmap_t *hm);

   // Return the mean number of entries in a bucket
   size_t ds_hmap_mean_entries (ds_hmap_t *hm);

   // Return the std deviation of entries in a bucket
   float ds_hmap_stddev_entries (ds_hmap_t *hm);

   // Return the min, max and range number of entries in a bucket
   size_t ds_hmap_min_entries (ds_hmap_t *hm);
   size_t ds_hmap_max_entries (ds_hmap_t *hm);
   size_t ds_hmap_range_entries (ds_hmap_t *hm);

   void ds_hmap_print_freq (ds_hmap_t *hm, const char *marker, FILE *outf);

#ifdef __cplusplus
};
#endif

#ifndef DS_HMAP_IMPLEMENTATION
// Hashmaps of <str,str> and <str,void *> are more common than others,
// thus for convenience a few inline functions are provided that deal
// specifically with these two common cases.
//
// The #ifndef above is to prevent these functions getting defined in the
// implementation file ds_hmap.c

LOCAL_INLINE
static const char *ds_hmap_set_str_str (ds_hmap_t *hm,
                                        const char *key, const char *data)
{
   return ds_hmap_set (hm, key,  strlen (key) + 1,
                           (void *)data, strlen (data) + 1);
}

LOCAL_INLINE
static bool ds_hmap_get_str_str (ds_hmap_t *hm,
                                 const char *key, char **data)
{
   size_t datalen;
   void *tmp = data;
   return ds_hmap_get (hm, key,  strlen (key) + 1,
                           tmp,  &datalen);
}

LOCAL_INLINE
static void ds_hmap_remove_str (ds_hmap_t *hm, const char *key)
{
   ds_hmap_remove (hm, key, strlen (key) + 1);
}

// TODO: Test this
LOCAL_INLINE
static const char *ds_hmap_set_str_ptr (ds_hmap_t *hm,
                                        const char *key,
                                        void *data)
{
   return ds_hmap_set (hm, key,  strlen (key) + 1,
                           data, sizeof data);
}

// TODO: Test this
LOCAL_INLINE
static bool ds_hmap_get_str_ptr (ds_hmap_t *hm,
                                 const char *key,
                                 void **data)
{
   size_t datalen = sizeof data;
   return ds_hmap_get (hm, key, strlen (key) + 1,
                           data, &datalen);
}


#endif

#endif
