#include <stdint.h>
#include <stdlib.h>

#include "ds_hmap.h"

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

static void entry_del (entry_t *e)
{
   if (!e)
      return;

   free (e->key);

   free (e);
}

static entry_t *entry_new (uint8_t flags, void *k, size_t klen,
                                          void *d, size_t dlen)
{
   bool error = true;
   entry_t *ret = NULL;

   if (!(ret = malloc (sizeof *ret)))
      goto errorexit;

   memset (ret, 0, sizeof *ret);

   ret->flags = flags;
   ret->keylen = klen;
   ret->datalen = dlen;

   if (!(ret->key = malloc (klen)))
      goto errorexit;

   memcpy (ret->key, k, klen);

   if (dlen <= sizeof ret->data) {
      ret->data = d;
   } else {
      memcpy (&ret->data, d, dlen);
   }

   error = false;

errorexit:

   if (error) {
      entry_del (ret);
      ret = NULL;
   }

   return ret;
}

/*
static entry_t *entry_reset (entry_t *e, uint8_t flags,
                                         void *k, size_t klen,
                                         void *d, size_t dlen)
{
   bool error = true;

   if (!e || !d)
      return NULL;

   e->flags = flags;

   free (e->key);
   if (!(e->key = malloc (klen)))
      goto errorexit;
   memcpy (e->key, k, klen);

   e->keylen = klen;
   e->data = d;
   e->datalen = dlen;

   error = false;

errorexit:
   if (error) {
      free (e->key);
      memset (e, 0, sizeof *e);
      return NULL;
   }

   return e;
}
*/

/* ******************************************************************
 * The data structure that stores a collection of entry_t elements.
 */
typedef struct bucket_t bucket_t;
struct bucket_t {
   size_t   nelems;
   entry_t *elems;
};

static void bucket_clear (bucket_t *b)
{
   if (!b)
      return;

   for (size_t i=0; i<b->nelems; i++) {
      free (b->elems[i].key);
   }
   free (b->elems);
   memset (b, 0, sizeof *b);
}

static entry_t *bucket_find_entry (bucket_t *b, const void *k, size_t klen)
{
   if (!b)
      return NULL;

   if (!k) {
      for (size_t i=0; i<b->nelems; i++) {
         if (!b->elems[i].key)
            return &b->elems[i];
      }
      return NULL;
   }

   for (size_t i=0; i<b->nelems; i++) {
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

   entry_t *tmp = realloc (b->elems, (b->nelems + 1) * (sizeof *tmp));
   if (!tmp)
      return NULL;

   b->elems = tmp;

   memset (&b->elems[b->nelems], 0, sizeof b->elems[b->nelems]);

   if (!(b->elems[b->nelems].key = malloc (klen)))
      return NULL;

   memcpy (b->elems[b->nelems].key, k, klen);
   b->elems[b->nelems].keylen = klen;
   b->elems[b->nelems].data = d;
   b->elems[b->nelems].datalen = dlen;

   ret = &b->elems[b->nelems];

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
static uint8_t rtab[] = {
   0x38, 0x1f, 0x65, 0x46, 0x24, 0x5f, 0x3e, 0x7d,
   0x29, 0x22, 0x5a, 0x62, 0x23, 0x3a, 0x47, 0x4e,
   0x20, 0x6c, 0x56, 0x4c, 0x7a, 0x7f, 0x40, 0xd9,
   0xdd, 0x1e, 0x69, 0x72, 0x3c, 0x3c, 0x52, 0x4d,
   0x14, 0xbc, 0x7a, 0x73, 0x23, 0x1a, 0x84, 0x53,
   0x72, 0x27, 0x42, 0x41, 0x77, 0x3c, 0x7b, 0x37,
   0x48, 0xdd, 0x20, 0x2f, 0x12, 0x35, 0x6f, 0x2d,
   0x74, 0xb9, 0x27, 0x3f, 0x41, 0x4a, 0x4b, 0x3f,
   0x28, 0x26, 0x54, 0x4d, 0x74, 0xdb, 0x32, 0x44,
   0x22, 0x51, 0x19, 0x60, 0xe4, 0x1e, 0x3a, 0x2f,
   0x27, 0x41, 0x59, 0x49, 0x2d, 0x15, 0xc4, 0x56,
   0xa0, 0x7a, 0x6e, 0x22, 0x44, 0x4b, 0x53, 0x41,
   0x16, 0xd3, 0x18, 0x5d, 0x3b, 0x46, 0x52, 0x11,
   0x35, 0xfe, 0x56, 0x5c, 0x67, 0x4f, 0x57, 0x71,
   0xcd, 0x30, 0x56, 0x19, 0x32, 0x14, 0x4b, 0x78,
   0x7a, 0x45, 0x4f, 0x60, 0x3e, 0x65, 0x1a, 0x7e,
   0xf2, 0x29, 0x11, 0x59, 0x61, 0x21, 0x67, 0x24,
   0x4d, 0x29, 0x39, 0x39, 0x91, 0x57, 0x28, 0x31,
   0x5a, 0x39, 0x4b, 0x10, 0x45, 0x3a, 0x7c, 0x1e,
   0x38, 0x34, 0x5d, 0x16, 0x27, 0x3d, 0x78, 0x75,
   0xc5, 0x13, 0x76, 0x23, 0x30, 0x55, 0x34, 0x3c,
   0x25, 0x38, 0x62, 0xa5, 0x23, 0x59, 0xc3, 0x13,
   0x96, 0x5a, 0x8e, 0x4f, 0x86, 0x31, 0x45, 0x26,
   0x56, 0x43, 0x66, 0x7f, 0x25, 0x6a, 0x5d, 0x69,
   0x32, 0x10, 0x1b, 0xcf, 0x72, 0x2c, 0x20, 0x6d,
   0x70, 0x15, 0x63, 0x47, 0x2f, 0x75, 0x2b, 0x6e,
   0x78, 0x59, 0x54, 0x3f, 0x71, 0x51, 0x50, 0x23,
   0x6e, 0x62, 0x1f, 0x7d, 0x1a, 0x5a, 0x4c, 0x33,
   0x1b, 0x58, 0x58, 0x69, 0x47, 0x49, 0xd1, 0x22,
   0xf0, 0x4c, 0x96, 0x1a, 0x40, 0xde, 0x69, 0x68,
   0x3a, 0x31, 0x2e, 0x58, 0x2a, 0x16, 0x28, 0x7e,
   0x60, 0x13, 0x37, 0x47, 0x41, 0xe8, 0x74, 0x4c,
};

static size_t make_hash (const void *k, size_t klen)
{
   const uint8_t *tmp = k;
   uint32_t ret = 0;

   for (size_t i=0; i<klen; i++) {
      ret = (ret << 5) + ret;
      ret = ret ^ rtab [*tmp++];
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

