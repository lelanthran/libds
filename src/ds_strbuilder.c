
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "ds_strbuilder.h"

#include "ds_array.h"


struct ds_strbuilder_t {
  ds_array_t *array;
};

ds_strbuilder_t *ds_strbuilder_new (const char *s, ...)
{
  va_list ap;
  va_start (ap, s);
  ds_strbuilder_t *ret = ds_strbuilder_newv (s, ap);
  va_end (ap);
  return ret;
}



ds_strbuilder_t *ds_strbuilder_newv (const char *s, va_list ap)
{
  bool error = true;
  ds_strbuilder_t *ret = calloc (1, sizeof *ret);
  if (!ret || !(ret->array = ds_array_new ())) {
    goto cleanup;
  }
  if (!ds_strbuilder_appendv (ret, s, ap))
    goto cleanup;

  error = false;
cleanup:
  if (error) {
    ds_strbuilder_del (ret);
    ret = NULL;
  }
  return ret;
}

void ds_strbuilder_del (ds_strbuilder_t *sb)
{
  if (!sb)
    return;

  size_t nitems = ds_array_length (sb->array);
  for (size_t i=0; i<nitems; i++) {
    free (ds_array_get (sb->array, i));
  }
  ds_array_del (sb->array);
  free (sb);
}

bool ds_strbuilder_append (ds_strbuilder_t *sb, const char *s, ...)
{
  va_list ap;
  va_start (ap, s);
  bool ret = ds_strbuilder_appendv (sb, s, ap);
  va_end (ap);
  return ret;
}

bool ds_strbuilder_appendv (ds_strbuilder_t *sb, const char *s, va_list ap)
{
  while (s) {
    size_t len = strlen (s);
    size_t nbytes = len + sizeof len; // Hold the size as well
    uint8_t *tmp = calloc (1, nbytes);
    if (!tmp)
      return false;
    memcpy (tmp, &len, sizeof len);
    memcpy (&tmp[sizeof len], s, len);
    if (!(ds_array_ins_tail (sb->array, tmp))) {
      free (tmp);
      return false;
    }
    s = va_arg (ap, const char *);
  }
  return true;
}

char *ds_strbuilder_build (ds_strbuilder_t *sb)
{
  char *ret = NULL;
  size_t retlen = 0;
  size_t nitems = ds_array_length (sb->array);
  for (size_t i=0; i<nitems; i++) {
    size_t len = 0;
    uint8_t *bytea = ds_array_get (sb->array, i);
    if (!bytea)
      return NULL;
    memcpy (&len, bytea, sizeof len);
    retlen += len;
  }

  if (!(ret = calloc (1, retlen + 1)))
    return NULL;

  size_t idx = 0;
  for (size_t i=0; i<nitems; i++) {
    size_t len = 0;
    uint8_t *bytea = ds_array_get (sb->array, i);
    if (!bytea) {
      free (ret);
      return NULL;
    }
    memcpy (&len, bytea, sizeof len);
    memcpy (&ret[idx], &bytea[sizeof len], len);
    idx += len;
  }
  return ret;
}






