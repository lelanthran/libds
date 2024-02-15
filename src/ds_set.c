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
   size_t len;
};

static void bucket_init (struct bucket_t *bucket)
{
   memset (bucket, 0, sizeof *bucket);
}

static void bucket_clear (struct bucket_t *bucket)
{
   free (bucket->array);
   memset (bucket, 0, sizeof *bucket);
}

static void *bucket_find (const struct bucket_t *bucket,  ds_set_cmp_t *fptr,
                          const void *o)
{
   if (!bucket)
      return NULL;

   for (size_t i=0; i<bucket->len; i++) {
      if ((fptr (o, bucket->array[i])) == 0)
         return (void *)bucket->array[i];
   }
   return NULL;
}

static const void *bucket_add (struct bucket_t *bucket, const void *object)
{
   const void **tmp = realloc (bucket->array, sizeof *bucket * (bucket->len + 1));
   if (!tmp)
      return NULL;

   bucket->array = tmp;
   bucket->array[bucket->len] = object;
   bucket->len++;

   return object;
}

static void bucket_remove (struct bucket_t *bucket,  ds_set_cmp_t *fptr,
                           const void *o)
{
   for (size_t i=0; i<bucket->len; i++) {
      if ((fptr (o, bucket->array[i])) == 0) {
         memmove (&bucket->array[i], &bucket->array[i+1],
               sizeof bucket->array[i] * (bucket->len - 1 - i));
         bucket->len--;
         return;
      }
   }
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

const void *ds_set_add (ds_set_t *set, const void *object, size_t object_length)
{
   if (!set)
      return NULL;

   size_t hash = hash_calc (object, object_length);
   size_t index = hash % set->nbuckets;
   if (!(bucket_find (&set->buckets[index], set->cmpfptr, object))) {
      if (!(bucket_add (&set->buckets[index], object))) {
         return NULL;
      }
   }

   return object;
}

void ds_set_remove (ds_set_t *set, const void *object, size_t object_length)
{
   size_t hash = hash_calc (object, object_length);
   size_t index = hash % set->nbuckets;
   bucket_remove (&set->buckets[index], set->cmpfptr, object);
}

void *ds_set_find (ds_set_t *set, const void *object, size_t object_length)
{
   size_t hash = hash_calc (object, object_length);
   size_t index = hash % set->nbuckets;
   return bucket_find (&set->buckets[index], set->cmpfptr, object);
}


void **ds_set_entries (ds_set_t *set)
{
   size_t nitems = 0;
   for (size_t i=0; i<set->nbuckets; i++) {
      nitems += set->buckets[i].len;
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
         entries[index++] = (void *)set->buckets[i].array[j];
      }
   }

   return entries;
}

