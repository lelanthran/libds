#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define DS_HMAP_IMPLEMENTATION
#include "ds_hmap.h"
#undef DS_HMAP_IMPLEMENTATION


/* ******************************************************************
 * The data structure that stores each key/value pair.
 */
#define FLAG_RFU0       (1 << 0)
#define FLAG_DELETE     (1 << 1)
#define FLAG_RFU2       (1 << 2)
#define FLAG_RFU3       (1 << 3)
#define FLAG_RFU4       (1 << 4)
#define FLAG_RFU5       (1 << 5)
#define FLAG_RFU6       (1 << 6)
#define FLAG_RFU7       (1 << 7)

typedef struct entry_t entry_t;
struct entry_t {
   uint8_t  flags;
   size_t   keylen;
   void    *key;
   size_t   datalen;
   void    *data;
};


/* ******************************************************************
 * The data structure that stores a collection of entry_t elements.
 */
typedef struct bucket_t bucket_t;
struct bucket_t {
   size_t   nelems;
   size_t   alen;
   entry_t *elems;
};

static void bucket_clear (bucket_t *b)
{
   if (!b)
      return;

   for (size_t i=0; i<b->alen; i++) {
      free (b->elems[i].key);
      b->elems[i].key = NULL;
      b->elems[i].keylen = 0;
   }
   free (b->elems);
   memset (b, 0, sizeof *b);
}

static entry_t *bucket_find_entry (bucket_t *b, const void *k, size_t klen)
{
   if (!b)
      return NULL;

   if (!k) {
      for (size_t i=0; i<b->alen; i++) {
         if (!b->elems[i].keylen)
            return &b->elems[i];
      }
      return NULL;
   }

   for (size_t i=0; i<b->alen; i++) {
      if (!b->elems[i].keylen)
         continue;
      size_t mlen = klen < b->elems[i].keylen ? klen : b->elems[i].keylen;
      if ((memcmp (b->elems[i].key, k, mlen))==0)
         return &b->elems[i];
   }

   return NULL;
}

static entry_t *bucket_new_entry (bucket_t *b, const void *k, size_t klen,
                                               void *d, size_t dlen)
{
   entry_t *ret = NULL;

   entry_t *tmp = realloc (b->elems, (b->alen + 1) * (sizeof *tmp));
   if (!tmp)
      return NULL;

   b->elems = tmp;

   memset (&b->elems[b->alen], 0, sizeof b->elems[b->alen]);

   if (!(b->elems[b->alen].key = malloc (klen)))
      return NULL;

   memcpy (b->elems[b->alen].key, k, klen);
   b->elems[b->alen].keylen = klen;
   b->elems[b->alen].data = d;
   b->elems[b->alen].datalen = dlen;

   ret = &b->elems[b->alen];

   b->alen++;
   b->nelems++;

   return ret;
}

static entry_t *bucket_set (bucket_t *b, const void *k, size_t klen,
                                         void *d, size_t dlen)
{
   entry_t *e = NULL;

   // Look for an existing entry
   e = bucket_find_entry (b, k, klen);
   if (!e) {
      // Doesn't exist, try to find an empty entry
      e = bucket_find_entry (b, NULL, 0);
      if (e) {
         if (!(e->key = malloc (klen)))
            return NULL;
         memcpy (e->key, k, klen);
         e->keylen = klen;
         b->nelems++;
      }
   }

   if (!e) {
      // No existing entry and no empty entries, create a new one
      return bucket_new_entry (b, k, klen, d, dlen);
   }

   e->data = d;
   e->datalen = dlen;

   return e;
}

/* ******************************************************************
 * The hashing functions
 */
static size_t make_hash (const void *k, size_t klen)
{
   const uint8_t *tmp = k;
   uint32_t ret = 0;

   for (size_t i=0; i<klen; i++) {
      ret = (ret << 5);
      ret = ret ^ *tmp;
      tmp++;
   }
   return ret;
}


/* ******************************************************************
 * The data structure that stores a collection of bucket_t elements.
 */
struct ds_hmap_t {
   int         errnum;
   char       *errmsg;

   size_t      nbuckets;
   bucket_t   *buckets;
};

ds_hmap_t *ds_hmap_new (size_t nbuckets)
{
   bool error = true;
   ds_hmap_t *ret = NULL;

   if (!(ret = malloc (sizeof *ret)))
      goto errorexit;

   memset (ret, 0, sizeof *ret);

   ret->nbuckets = nbuckets;
   if (!(ret->buckets = malloc ((sizeof *ret->buckets) * nbuckets)))
      goto errorexit;

   memset (ret->buckets, 0, (sizeof *ret->buckets) * nbuckets);

   error = false;

errorexit:
   if (error) {
      ds_hmap_del (ret);
      ret = NULL;
   }

   return ret;
}

void ds_hmap_del (ds_hmap_t *hm)
{
   if (!hm)
      return;

   for (size_t i=0; i<hm->nbuckets; i++) {
      bucket_clear (&hm->buckets[i]);
   }
   free (hm->buckets);
   free (hm);
}

static const char *find_errmsg (ds_hmap_error_t code)
{
   static const struct {
      ds_hmap_error_t   code;
      const char       *msg;
   } msgs[] = {
      { ds_hmap_ENONE,       "Success"          },
      { ds_hmap_ENULLOBJ,    "Null Object"      },
      { ds_hmap_EOOM,        "Out of memory"    },
   };

   static const size_t nmsgs = sizeof msgs / sizeof msgs[0];

   for (size_t i=0; i<nmsgs; i++) {
      if (msgs[i].code == code)
         return msgs[i].msg;
   }

   return "Unknown error code";
}

void ds_hmap_lasterr (ds_hmap_t *hm,
                      ds_hmap_error_t *err, const char **errmsg)
{
   if (!err && !errmsg)
      return;

   if (!hm) {
      if (err)    *err = ds_hmap_ENULLOBJ;
      if (errmsg) *errmsg = find_errmsg (ds_hmap_ENULLOBJ);
      return;
   }

   if (err)    *err = hm->errnum;
   if (errmsg) *errmsg = find_errmsg (hm->errnum);
}

const void *ds_hmap_set (ds_hmap_t *hm, const void *key,  size_t keylen,
                                        void *data, size_t datalen)
{
   bool error = true;

   if (!hm || !key || !data)
      return NULL;

   uint32_t hash = make_hash (key, keylen) % hm->nbuckets;
   entry_t *e = NULL;

   if (!(e = bucket_set (&hm->buckets[hash], key, keylen, data, datalen)))
      goto errorexit;

   error = false;

errorexit:

   return error ? NULL : e->key;
}

bool ds_hmap_get (ds_hmap_t *hm, const void *key,  size_t keylen,
                                 void **data,      size_t *datalen)
{
   bool error = true;

   if (!hm || !key || !data)
      return NULL;

   uint32_t hash = make_hash (key, keylen) % hm->nbuckets;
   const entry_t *entry = NULL;

   if (!(entry = bucket_find_entry (&hm->buckets[hash], key, keylen)))
      goto errorexit;

   (*data) = entry->data;
   (*datalen) = entry->datalen;

   error = false;

errorexit:

   return !error;
}

void ds_hmap_remove (ds_hmap_t *hm, const void *key, size_t keylen)
{
   if (!hm || !key || !keylen)
      return;

   uint32_t hash = make_hash (key, keylen) % hm->nbuckets;
   entry_t *e = bucket_find_entry (&hm->buckets[hash], key, keylen);
   if (!e)
      return;

   free (e->key);
   memset (e, 0, sizeof *e);
   hm->buckets[hash].nelems--;
}

size_t ds_hmap_keys (ds_hmap_t *hm, void ***keys, size_t **keylens)
{
   if (!hm)
      return (size_t)-1;

   bool error = true;

   void **k = NULL;
   size_t *kl = NULL;
   size_t ret = ds_hmap_num_entries (hm);

   k = malloc (sizeof *k * ret);
   kl = malloc (sizeof *kl * ret);

   if (!k || !kl)
      goto errorexit;

   size_t index = 0;
   for (size_t i=0; i<hm->nbuckets; i++) {
      for (size_t j=0; j<hm->buckets[i].alen; j++) {
         if (!hm->buckets[i].elems[j].keylen)
            continue;
         k[index] = hm->buckets[i].elems[j].key;
         kl[index] = hm->buckets[i].elems[j].keylen;
         index++;
      }
   }

   if (keys) {
      *keys = k;
   } else {
      free (k);
      k = NULL;
   }

   if (keylens) {
      *keylens = kl;
   } else {
      free (kl);
      kl = NULL;
   }

   error = false;

errorexit:

   if (error) {
      free (k);
      free (kl);
      ret = (size_t)-1;
   }

   return ret;
}

/* ******************************************************************
 * The statistics functions
 */

float ds_hmap_load (ds_hmap_t *hm)
{
   if (!hm)
      return 0.0;

   float numer = ds_hmap_num_entries (hm);
   float denom = ds_hmap_num_buckets (hm);
   return numer / denom;
}

size_t ds_hmap_num_buckets (ds_hmap_t *hm)
{
   return hm ? hm->nbuckets : 0;
}

size_t ds_hmap_num_entries (ds_hmap_t *hm)
{
   if (!hm)
      return 0;

   size_t sum = 0;
   for (size_t i=0; i<hm->nbuckets; i++) {
      sum += hm->buckets[i].nelems;
   }

   return sum;
}

size_t ds_hmap_mean_entries (ds_hmap_t *hm)
{
   if (!hm)
      return 0;

   return ds_hmap_num_entries (hm) / ds_hmap_num_buckets (hm);
}

float ds_hmap_stddev_entries (ds_hmap_t *hm)
{
   if (!hm)
      return 0;

   float avg = ds_hmap_load (hm);
   float dev = 0.0;

   for (size_t i=0; i<hm->nbuckets; i++) {
      float diff = avg - hm->buckets[i].nelems;
      dev += diff * diff;
   }

   float stddev = sqrtf (dev / hm->nbuckets);

   return stddev;
}

size_t ds_hmap_min_entries (ds_hmap_t *hm)
{
   if (!hm)
      return 0;

   size_t ret = (size_t)-1;

   for (size_t i=0; i<hm->nbuckets; i++) {
      if (hm->buckets[i].nelems < ret)
         ret = hm->buckets[i].nelems;
   }
   return ret;
}

size_t ds_hmap_max_entries (ds_hmap_t *hm)
{
   if (!hm)
      return 0;

   size_t ret = 0;

   for (size_t i=0; i<hm->nbuckets; i++) {
      // TODO: Remove this after testing.
      printf ("%zu : %zu\n", i, hm->buckets[i].nelems);
      if (hm->buckets[i].nelems > ret)
         ret = hm->buckets[i].nelems;
   }
   return ret;
}


