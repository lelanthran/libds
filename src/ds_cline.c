#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "ds_str.h"
#include "ds_kvpair.h"
#include "ds_cline.h"



struct ds_cline_t {
  size_t nargs;
  char **args;

  size_t nflags;
  ds_kvpair_t **flags;
};


ds_cline_t *ds_cline_parse (int argc, char **argv)
{
  bool error = true;
  ds_cline_t *ret = NULL;

  if (!(ret = calloc (1, sizeof *ret)))
    goto cleanup;

  size_t nargs = 0;
  size_t nflags = 0;

  for (int i=1; i<argc; i++) {
    if ((strncmp (argv[i], "--", 2)) == 0)
      nflags++;
    else
      nargs++;
  }

  // We don't use the extra element, but it allows nargs/nflags to be
  // zero and still get a valid pointer. Without this, calloc is allowed to
  // return NULL because it's a zero-length array, and then the function fails
  // with an OOM error because allocating a zero-length array resulted in
  // NULL. With the +1 there is always a valid poiinter returned, even though
  // we never use it.
  if (!(ret->args = calloc (nargs + 1, sizeof *ret->args)) ||
      !(ret->flags = calloc (nflags + 1, sizeof *ret->flags)))
    goto cleanup;

  ret->nargs = nargs;
  ret->nflags = nflags;

  size_t idx_args = 0,
         idx_flags = 0;

  for (int i=1; i<argc; i++) {
    if ((strncmp (argv[i], "--", 2)) == 0) {
      const char *name = &argv[i][2];
      char *value = strchr (name, '=');
      if (value) {
        *value++ = 0;
      } else {
        value = "";
      }
      if (!(ret->flags[idx_flags++] = ds_kvpair_new (name, value)))
        goto cleanup;
    } else {
      if (!(ret->args[idx_args++] = ds_str_dup (argv[i])))
        goto cleanup;
    }
  }

  error = false;
cleanup:
  if (error) {
    ds_cline_del (ret);
    ret = NULL;
  }
  return ret;
}


void ds_cline_del (ds_cline_t *cline)
{
  if (!cline)
    return;

  for (size_t i=0; i<cline->nargs; i++) {
    free (cline->args[i]);
  }
  free (cline->args);

  for (size_t i=0; i<cline->nflags; i++) {
    ds_kvpair_del (cline->flags[i]);
  }
  free (cline->flags);

  free (cline);
}



size_t ds_cline_arg_count (const ds_cline_t *cline)
{
  return cline ? cline->nargs : 0;
}

size_t ds_cline_flag_count (const ds_cline_t *cline)
{
  return cline ? cline->nflags : 0;
}



const char *ds_cline_arg_get (ds_cline_t *cline, size_t i)
{
  return cline && i < cline->nargs ? cline->args[i] : NULL;
}

const ds_kvpair_t *ds_cline_flag_get (ds_cline_t *cline, size_t i)
{
  return cline && i < cline->nflags ? cline->flags[i] : NULL;
}




bool ds_cline_arg_test (const ds_cline_t *cline, const char *arg)
{
  if (!cline || !arg)
       return false;

  for (size_t i=0; i < cline->nargs; i++) {
    if ((strcmp (cline->args[i], arg)) == 0)
      return true;
  }
  return false;
}

const char *ds_cline_flag_value (const ds_cline_t *cline, const char *flag)
{
  if (!cline || !flag)
    return NULL;

  for (size_t i=0; i < cline->nflags; i++) {
    const char *key = ds_kvpair_key_get (cline->flags[i]);
    if (key && (strcmp (key, flag)) == 0)
      return ds_kvpair_value_get (cline->flags[i]);
  }
  return NULL;
}

const char **ds_cline_flag_values (ds_cline_t *cline, const char *flag)
{
  if (!cline || !flag)
    return NULL;

  // Allocate enough to store every flag, not just the one specified. We will
  // use only a subset of this, but that's fine too.
  printf ("nflags: %zu\n", cline->nflags);
  const char **ret = calloc (cline->nflags + 1, sizeof *ret);
  if (!ret)
    return false;

  size_t idx = 0;
  for (size_t i=0; i < cline->nflags; i++) {
    const char *key = ds_kvpair_key_get (cline->flags[i]);
    if (key && (strcmp (key, flag)) == 0)
      ret[idx++] = ds_kvpair_value_get (cline->flags[i]);
  }
  return ret;
}
