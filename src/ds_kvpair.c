#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#include "ds_kvpair.h"




// Not pulling in an entire other module just for a single function
static char *lstrdup (const char *src)
{
  if (!src)
    return NULL;

  size_t nbytes = strlen (src);
  char *ret = malloc (nbytes + 1);
  if (!ret)
    return NULL;

  return strcpy (ret, src);
}

static const char *str_replace (char **old, const char *new)
{
  char *tmp = lstrdup (new);
  if (!old)
    return NULL;

  if (tmp == NULL && new != NULL)
    return NULL;

  free (*old);
  *old = tmp;
  return *old;
}




struct ds_kvpair_t {
  char *key;
  char *value;
};


ds_kvpair_t *ds_kvpair_new (const char *key, const char *value)
{
  ds_kvpair_t *ret = calloc (1, sizeof *ret);
  if (ret &&
      (ret->key = lstrdup (key)) &&
      (ret->value = lstrdup (value))) {
    return ret;
  }

  ds_kvpair_del (ret);
  return NULL;
}

void ds_kvpair_del (ds_kvpair_t *kv)
{
  if (!kv)
    return;
  free (kv->key);
  free (kv->value);
  free (kv);
}

const char *ds_kvpair_key_get (const ds_kvpair_t *kv)
{
  return kv ? kv->key : NULL;
}

const char *ds_kvpair_key_set (ds_kvpair_t *kv, const char *key)
{
  if (!kv)
    return NULL;

  return str_replace (&kv->key, key);
}

const char *ds_kvpair_value_get (const ds_kvpair_t *kv)
{
  return kv ? kv->value : NULL;
}

const char *ds_kvpair_value_set (ds_kvpair_t *kv, const char *value)
{
  if (!kv)
    return NULL;

  return str_replace (&kv->value, value);
}

