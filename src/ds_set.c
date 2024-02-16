#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ds_set.h"

#define LOG(...)     do {\
   fprintf (stderr, "%s:%i: in `%s`:", __FILE__, __LINE__, __func__);\
   fprintf (stderr, __VA_ARGS__);\
} while (0);


static size_t hash_calc (const void *o, size_t olen)
{
   const uint8_t *bytes = o;
   uint32_t hash = 0;
   for (size_t i=0; i<olen; i++) {
      uint8_t carry = (hash & 0xff000000);
      hash = (hash << 8) ^ (carry ^ bytes[i]);
   }
   return hash;
}

/* ********************************************************************** */

struct bucket_t {
   const void **array;
   size_t *olengths;
   size_t len;
};

static void bucket_init (struct bucket_t *bucket)
{
   memset (bucket, 0, sizeof *bucket);
}

static void bucket_clear (struct bucket_t *bucket)
{
   free (bucket->array);
   free (bucket->olengths);
   memset (bucket, 0, sizeof *bucket);
}

static void *bucket_find (const struct bucket_t *bucket,  ds_set_cmp_t *fptr,
                          const void *o, size_t olen)
{
   if (!bucket)
      return NULL;

   for (size_t i=0; i<bucket->len; i++) {
      if ((fptr (o, olen, bucket->array[i], bucket->olengths[i])) == 0)
         return (void *)bucket->array[i];
   }
   return NULL;
}

static const void *bucket_add (struct bucket_t *bucket,
                               const void *object, size_t object_length)
{
   size_t alen = bucket->len;
   const void **t1 = realloc (bucket->array, sizeof (*bucket->array) * (alen + 1));
   if (!t1)
      return NULL;
   bucket->array = t1;

   size_t *t2 = realloc (bucket->olengths, sizeof (*bucket->olengths) * (alen + 1));
   if (!t2) {
      return NULL;
   }
   bucket->olengths = t2;

   bucket->array[bucket->len] = object;
   bucket->olengths[bucket->len] = object_length;
   bucket->len++;

   return object;
}

static const void *bucket_remove (struct bucket_t *bucket,  ds_set_cmp_t *fptr,
                                  const void *o, size_t olen)
{
   for (size_t i=0; i<bucket->len; i++) {
      if ((fptr (o, olen, bucket->array[i], bucket->olengths[i])) == 0) {
         const void *ret = bucket->array[i];
         memmove (&bucket->array[i], &bucket->array[i+1],
               sizeof bucket->array[i] * (bucket->len - 1 - i));
         memmove (&bucket->olengths[i], &bucket->olengths[i+1],
               sizeof bucket->olengths[i] * (bucket->len - 1 - i));
         bucket->len--;
         return ret;
      }
   }
   return NULL;
}

/* ********************************************************************** */

struct ds_set_t {
   size_t nbuckets;
   struct bucket_t *buckets;
   ds_set_cmp_t *cmpfptr;
};

ds_set_t *ds_set_new (ds_set_cmp_t *cmpfunc, size_t nbuckets)
{
   ds_set_t *ret = calloc (1, sizeof *ret);
   if (!ret || !(ret->buckets = calloc (nbuckets, sizeof *ret->buckets))) {
      ds_set_del (ret);
      return NULL;
   }
   for (size_t i=0; i<nbuckets; i++) {
      bucket_init (&ret->buckets[i]);
   }

   ret->nbuckets = nbuckets;
   ret->cmpfptr = cmpfunc;
   return ret;
}

void ds_set_del (ds_set_t *set)
{
   if (!set)
      return;

   for (size_t i=0; i<set->nbuckets; i++) {
      bucket_clear (&set->buckets[i]);
   }

   free (set->buckets);
   free (set);
}

int ds_set_add (ds_set_t *set, const void *object, size_t object_length)
{
   if (!set)
      return -1;

   size_t hash = hash_calc (object, object_length);
   size_t index = hash % set->nbuckets;
   if (!(bucket_find (&set->buckets[index], set->cmpfptr, object, object_length))) {
      if (!(bucket_add (&set->buckets[index], object, object_length))) {
         return -1;
      }
      return 1;
   }

   return 0;
}

const void *ds_set_remove (ds_set_t *set, const void *object, size_t object_length)
{
   size_t hash = hash_calc (object, object_length);
   size_t index = hash % set->nbuckets;
   return bucket_remove (&set->buckets[index], set->cmpfptr, object, object_length);
}

void *ds_set_find (ds_set_t *set, const void *object, size_t object_length)
{
   size_t hash = hash_calc (object, object_length);
   size_t index = hash % set->nbuckets;
   return bucket_find (&set->buckets[index], set->cmpfptr, object, object_length);
}


void **ds_set_entries (const ds_set_t *set, size_t **lengths)
{
   size_t nitems = 0;
   for (size_t i=0; i<set->nbuckets; i++) {
      nitems += set->buckets[i].len;
   }
   if (lengths) {
      if (!(*lengths = calloc (nitems + 1, sizeof (size_t)))) {
         return NULL;
      }
   }
   if (nitems == 0) {
      return calloc (1, sizeof (void *));
   }

   void **entries = calloc (nitems + 1, sizeof *entries);
   if (!entries)
      return NULL;

   size_t index = 0;
   for (size_t i=0; i<set->nbuckets; i++) {
      for (size_t j=0; j<set->buckets[i].len; j++) {
         if (lengths) {
            (*lengths)[index] = set->buckets[i].olengths[j];
         }
         entries[index++] = (void *)set->buckets[i].array[j];
      }
   }

   return entries;
}

void ds_set_fptr (ds_set_t *set, void (*fptr) (void *))
{
   if (!set || !fptr)
      return;

   void **entries = ds_set_entries (set, NULL);
   if (!entries)
      return;

   for (size_t i=0; entries[i]; i++) {
      fptr (entries[i]);
   }
   free (entries);
}

void ds_set_iterate (ds_set_t *set, void (*fptr) (const void *, void *),
                     void *param)
{
   if (!set || !fptr)
      return;

   void **entries = ds_set_entries (set, NULL);
   if (!entries)
      return;

   for (size_t i=0; entries[i]; i++) {
      fptr (entries[i], param);
   }
   free (entries);
}

void **ds_set_map (ds_set_t *set, void *(*fptr) (const void *, void *),
                   void *param)
{
   if (!set || !fptr)
      return NULL;

   void **ret = NULL;
   void **entries = ds_set_entries (set, NULL);
   if (!entries) {
      ret = calloc (1, sizeof *ret);
      return ret;
   }

   size_t nentries = 1;
   for (size_t i=0; entries[i]; i++) {
      nentries++;
   }

   if (!(ret = calloc (nentries, sizeof *ret))) {
      free (entries);
      return NULL;
   }

   for (size_t i=0; entries[i]; i++) {
      ret[i] = fptr (entries[i], param);
   }

   free (entries);
   return ret;
}


ds_set_t *ds_set_filter (const ds_set_t *set,
                         bool (*predicate) (const void *, void *),
                         void *param)
{
   if (!set || !predicate)
      return NULL;

   ds_set_t *ret = ds_set_new (set->cmpfptr, set->nbuckets);
   void **entries = ds_set_entries (set, NULL);
   if (!entries) {
      ret = calloc (1, sizeof *ret);
      return ret;
   }

   size_t nentries = 1;
   for (size_t i=0; entries[i]; i++) {
      nentries++;
   }

   if (!(ret = calloc (nentries, sizeof *ret))) {
      free (entries);
      return NULL;
   }

   for (size_t i=0; entries[i]; i++) {
      if ((predicate (entries[i], param))) {
         if ((ds_set_add (ret, entries[i], 0)) < 0) { // TODO: This looks like a bug
            free (entries);
            ds_set_del (ret);
            return NULL;
         }
      }
   }

   free (entries);
   return ret;
}

