#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ds_kvpair.h"

int main (void)
{
  int ret = EXIT_FAILURE;

  ds_kvpair_t *pair = ds_kvpair_new ("key1", "value1");
  if (!pair) {
    fprintf (stderr, "Failed to create a kv pair\n");
    goto cleanup;
  }

  printf ("Initial value [%s:%s]\n",
          ds_kvpair_key_get (pair),
          ds_kvpair_value_get (pair));

  ds_kvpair_key_set (pair, "key2");
  printf ("Key change [%s:%s]\n",
          ds_kvpair_key_get (pair),
          ds_kvpair_value_get (pair));

  ds_kvpair_value_set (pair, "value2");
  printf ("Value change [%s:%s]\n",
          ds_kvpair_key_get (pair),
          ds_kvpair_value_get (pair));

  ds_kvpair_key_set (pair, NULL);
  printf ("Key is NULL [%s:%s]\n",
          ds_kvpair_key_get (pair),
          ds_kvpair_value_get (pair));

  ds_kvpair_value_set (pair, NULL);
  printf ("Value is NULL [%s:%s]\n",
          ds_kvpair_key_get (pair),
          ds_kvpair_value_get (pair));

  ds_kvpair_key_set (pair, "key3");
  printf ("Key is reset [%s:%s]\n",
          ds_kvpair_key_get (pair),
          ds_kvpair_value_get (pair));

  ds_kvpair_value_set (pair, "value3");
  printf ("Value is reset [%s:%s]\n",
          ds_kvpair_key_get (pair),
          ds_kvpair_value_get (pair));

  ret = EXIT_SUCCESS;
cleanup:
  ds_kvpair_del (pair);
  return ret;
}


